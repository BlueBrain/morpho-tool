#include "morphologyASC.h"

#include <fstream>

#include <morphio/mut/morphology.h>
#include <morphio/mut/section.h>


#include "lex.cpp"

namespace morphio {
namespace readers {
namespace asc {
namespace {

/**
   Contain header info about the root S-exps
**/
struct Header {
    Header()
        : token(static_cast<Token>(+Token::STRING))
        , parent_id(-1) {}
    Token token;
    std::string label;
    int32_t parent_id;
};

bool is_eof(Token type) {
    return type == Token::EOF_;
}

bool is_end_of_branch(Token type) {
    return (type == Token::GENERATED || type == Token::HIGH || type == Token::INCOMPLETE ||
            type == Token::LOW || type == Token::NORMAL || type == Token::MIDPOINT ||
            type == Token::ORIGIN);
}

bool is_neurite_type(Token id) {
    return (id == Token::AXON || id == Token::APICAL || id == Token::DENDRITE ||
            id == Token::CELLBODY);
}

bool is_end_of_section(Token id) {
    return (id == Token::RPAREN || id == Token::PIPE);
}

bool skip_sexp(size_t id) {
    return (id == +Token::WORD || id == +Token::COLOR || id == +Token::GENERATED ||
            id == +Token::HIGH || id == +Token::INCOMPLETE || id == +Token::LOW ||
            id == +Token::NORMAL);
}
}  // namespace

class NeurolucidaParser
{
  public:
    explicit NeurolucidaParser(const std::string& uri)
        : uri_(uri)
        , lex_(uri)
        , debugInfo_(uri)
        , err_(uri) {}

    NeurolucidaParser(NeurolucidaParser const&) = delete;
    NeurolucidaParser& operator=(NeurolucidaParser const&) = delete;

    morphio::mut::Morphology& parse() {
        std::ifstream ifs(uri_);
        std::string input((std::istreambuf_iterator<char>(ifs)),
                          (std::istreambuf_iterator<char>()));

        lex_.start_parse(input);

        parse_root_sexps();

        return nb_;
    }

  private:
    std::tuple<Point, floatType> parse_point(NeurolucidaLexer& lex) {
        lex.expect(Token::LPAREN, "Point should start in LPAREN");
        std::array<morphio::floatType, 4> point{};  // X,Y,Z,R
        for (auto& p : point) {
            try {
#ifdef MORPHIO_USE_DOUBLE
                p = std::stod(lex.consume()->str());
#else
                p = std::stof(lex.consume()->str());
#endif
            } catch (const std::invalid_argument&) {
                throw RawDataError(err_.ERROR_PARSING_POINT(lex.line_num(), lex.current()->str()));
            }
        }

        lex.consume();

        if (lex.current()->id == +Token::WORD) {
            lex.consume(Token::WORD);
        }

        lex.consume(Token::RPAREN, "Point should end in RPAREN");

        return std::tuple<Point, floatType>({point[0], point[1], point[2]}, point[3]);
    }

    bool parse_neurite_branch(Header& header) {
        lex_.consume(Token::LPAREN, "New branch should start with LPAREN");

        bool ret = true;
        while (true) {
            ret &= parse_neurite_section(header);
            if (lex_.ended() ||
                (lex_.current()->id != +Token::PIPE && lex_.current()->id != +Token::LPAREN)) {
                break;
            }
            lex_.consume();
        }
        lex_.consume(Token::RPAREN, "Branch should end with RPAREN");
        return ret;
    }

    int32_t _create_soma_or_section(const Header& header,
                                    std::vector<Point>& points,
                                    std::vector<morphio::floatType>& diameters) {
        lex_.current_section_start_ = lex_.line_num();
        int32_t return_id;
        morphio::Property::PointLevel properties;
        properties._points = points;
        properties._diameters = diameters;
        if (header.token == Token::STRING) {
            Property::Marker marker;
            marker._pointLevel = properties;
            marker._label = header.label;
            nb_.addMarker(marker);
            return_id = -1;
        } else if (header.token == Token::CELLBODY) {
            if (!nb_.soma()->points().empty())
                throw SomaError(err_.ERROR_SOMA_ALREADY_DEFINED(lex_.line_num()));
            nb_.soma()->properties() = properties;
            return_id = -1;
        } else {
            SectionType section_type = TokenSectionTypeMap.at(header.token);
            insertLastPointParentSection(header.parent_id, properties);

            // Condition to remove single point section that duplicate parent
            // point See test_single_point_section_duplicate_parent for an
            // example
            if (header.parent_id > -1 && properties._points.size() == 1) {
                return_id = header.parent_id;
            } else {
                std::shared_ptr<morphio::mut::Section> section;
                if (header.parent_id > -1)
                    section = nb_.section(static_cast<unsigned int>(header.parent_id))
                                  ->appendSection(properties, section_type);
                else
                    section = nb_.appendRootSection(properties, section_type);
                return_id = static_cast<int>(section->id());
                debugInfo_.setLineNumber(section->id(),
                                         static_cast<unsigned int>(lex_.current_section_start_));
            }
        }
        points.clear();
        diameters.clear();

        return return_id;
    }

    /*
      Add the last point of parent section to the beginning of this section
      if not already present.
      See https://github.com/BlueBrain/MorphIO/pull/221

      The diameter is taken from the child section next point as does NEURON.
      Here is the spec:
      https://bbpteam.epfl.ch/project/issues/browse/NSETM-1178?focusedCommentId=135030&page=com.atlassian.jira.plugin.system.issuetabpanels%3Acomment-tabpanel#comment-135030

      In term of diameters, the result should look like the right picture of:
      https://github.com/BlueBrain/NeuroM/issues/654#issuecomment-332864540

     The idea is that these two structures should represent the same morphology:

     (3 -8 0 5)     and          (3 -8 0 5)
     (3 -10 0 5)                 (3 -10 0 5)
     (                           (
       (0 -10 0 2)                 (3 -10 0 2)  <-- duplicate parent point, note that the
       (-3 -10 0 2)                (0 -10 0 2)      diameter is 2 and not 5
       |                           (-3 -10 0 2)
       (6 -10 0 2)                 |
       (9 -10 0 2)                 (3 -10 0 2)  <-- duplicate parent point, note that the
     )                             (6 -10 0 2)      diameter is 2 and not 5
                                   (9 -10 0 2)
                                 )
     */
    void insertLastPointParentSection(int32_t parentId,
                                      morphio::Property::PointLevel& properties,
                                      std::vector<morphio::floatType>& diameters) {
        if (parentId < 0)  // Discard root sections
            return;
        auto parent = nb_.section(static_cast<unsigned int>(parentId));
        auto lastParentPoint = parent->points()[parent->points().size() - 1];
        auto childSectionNextDiameter = diameters[0];

        if (lastParentPoint == properties._points[0])
            return;

        properties._points.insert(properties._points.begin(), lastParentPoint);
        properties._diameters.insert(properties._diameters.begin(), childSectionNextDiameter);
    }

    /**
       Parse the root sexp until finding the first sexp containing numbers
    **/
    Header parse_root_sexp_header() {
        Header header;

        while (true) {
            const auto id = static_cast<Token>(lex_.current()->id);
            const size_t peek_id = lex_.peek()->id;

            if (is_eof(id)) {
                throw RawDataError(err_.ERROR_EOF_IN_NEURITE(lex_.line_num()));
            } else if (id == Token::WORD) {
                lex_.consume();
            } else if (id == Token::STRING) {
                header.label = lex_.current()->str();
                // Get rid of quotes
                header.label = header.label.substr(1, header.label.size() - 2);
                lex_.consume();
            } else if (id == Token::RPAREN) {
                return header;
            } else if (id == Token::LPAREN) {
                if (skip_sexp(peek_id)) {
                    // skip words/strings/markers
                    lex_.consume_until_balanced_paren();
                } else if (is_neurite_type(static_cast<Token>(peek_id))) {
                    header.token = static_cast<Token>(peek_id);
                    lex_.consume();  // Advance to NeuriteType
                    lex_.consume();
                    lex_.consume(Token::RPAREN, "New Neurite should end in RPAREN");
                } else if (peek_id == +Token::NUMBER) {
                    return header;
                } else {
                    throw RawDataError(
                        err_.ERROR_UNKNOWN_TOKEN(lex_.line_num(), lex_.peek()->str()));
                }
            } else {
                throw RawDataError(
                    err_.ERROR_UNKNOWN_TOKEN(lex_.line_num(), lex_.current()->str()));
            }
        }
    }


    bool parse_neurite_section(Header header) {
        Points points;
        std::vector<morphio::floatType> diameters;
        auto section_id = static_cast<int>(nb_.sections().size());

        while (true) {
            const auto id = static_cast<Token>(lex_.current()->id);
            const size_t peek_id = lex_.peek()->id;

            if (is_eof(id)) {
                throw RawDataError(err_.ERROR_EOF_IN_NEURITE(lex_.line_num()));
            } else if (is_end_of_section(id)) {
                if (!points.empty()) {
                    _create_soma_or_section(header, points, diameters);
                }
                return true;
            } else if (is_end_of_branch(id)) {
                lex_.consume();
            } else if (id == Token::LSPINE) {
                // skip spines
                while (!lex_.ended() && static_cast<Token>(lex_.current()->id) != Token::RSPINE) {
                    lex_.consume();
                }
                lex_.consume(Token::RSPINE, "Must be end of spine");
            } else if (id == Token::LPAREN) {
                if (skip_sexp(peek_id)) {
                    // skip words/strings/markers
                    lex_.consume_until_balanced_paren();
                } else if (peek_id == +Token::NUMBER) {
                    Point point;
                    floatType radius;
                    std::tie(point, radius) = parse_point(lex_);
                    points.push_back(point);
                    diameters.push_back(radius);
                } else if (peek_id == +Token::LPAREN) {
                    if (!points.empty()) {
                        section_id = _create_soma_or_section(header, points, diameters);
                    }
                    Header child_header = header;
                    child_header.parent_id = section_id;
                    parse_neurite_branch(child_header);
                } else {
                    throw RawDataError(
                        err_.ERROR_UNKNOWN_TOKEN(lex_.line_num(), lex_.peek()->str()));
                }
            } else {
                throw RawDataError(err_.ERROR_UNKNOWN_TOKEN(lex_.line_num(), lex_.peek()->str()));
            }
        }
    }

    void parse_root_sexps() {
        // parse the top level blocks, and if they are a neurite, otherwise skip
        while (!lex_.ended()) {
            if (static_cast<Token>(lex_.current()->id) == Token::LPAREN) {
                lex_.consume();
                Header header = parse_root_sexp_header();
                if (lex_.current()->id != +Token::RPAREN) {
                    parse_neurite_section(header);
                }
            }

            if (!lex_.ended())
                lex_.consume();
        }
    }

    morphio::mut::Morphology nb_;

    std::string uri_;
    NeurolucidaLexer lex_;

  public:
    DebugInfo debugInfo_;

  private:
    ErrorMessages err_;
};

Property::Properties load(const std::string& uri, unsigned int options) {
    NeurolucidaParser parser(uri);

    morphio::mut::Morphology& nb_ = parser.parse();
    nb_.sanitize(parser.debugInfo_);
    nb_.applyModifiers(options);

    Property::Properties properties = nb_.buildReadOnly();
    properties._cellLevel._cellFamily = NEURON;
    properties._cellLevel._version = MORPHOLOGY_VERSION_ASC_1;
    return properties;
}

}  // namespace asc
}  // namespace readers
}  // namespace morphio
