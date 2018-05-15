#include <morphio/errorMessages.h>

namespace morphio
{
namespace plugin
{

const std::string ErrorMessages::errorMsg(int lineNumber, ErrorLevel errorLevel, std::string msg) const {
        return "\n" +
                (_uri.empty() ? "" : errorLink(lineNumber, errorLevel) + "\n")
                + msg;
}


////////////////////////////////////////////////////////////////////////////////
//              ERRORS
////////////////////////////////////////////////////////////////////////////////

const std::string ErrorMessages::ERROR_OPENING_FILE() const {
    return "Error opening morphology file:\n" + errorMsg(0, ErrorLevel::ERROR);
};

const std::string ErrorMessages::ERROR_LINE_NON_PARSABLE(int lineNumber) const {
    return errorMsg(lineNumber, ErrorLevel::ERROR, "Unable to parse this line");
}

const std::string ErrorMessages::ERROR_MULTIPLE_SOMATA(const std::vector<Sample>& somata) const {
    std::string msg("Multiple somata found: ");
    for(auto soma: somata)
        msg += "\n" + errorMsg(soma.lineNumber, ErrorLevel::ERROR);
    return msg;
}

const std::string ErrorMessages::ERROR_MISSING_PARENT(const Sample& sample) const {
    return errorMsg(sample.lineNumber, ErrorLevel::ERROR,
                    "Sample id: " + std::to_string(sample.id) +
                    " refers to non-existant parent ID: " +
                    std::to_string(sample.parentId));
}

std::string ErrorMessages::ERROR_SOMA_BIFURCATION(const Sample& sample) const {
    return errorMsg(sample.lineNumber, ErrorLevel::ERROR,
                    "Found soma bifurcation");
}

std::string ErrorMessages::ERROR_SOMA_WITH_NEURITE_PARENT(const Sample& sample) const {
    return errorMsg(sample.lineNumber, ErrorLevel::ERROR,
                    "Found a soma point with a neurite as parent");
};

const std::string ErrorMessages::ERROR_REPEATED_ID(const Sample& originalSample, const Sample& newSample) const {
    return errorMsg(newSample.lineNumber, ErrorLevel::WARNING,
                    "Repeated ID: " + std::to_string(originalSample.id)) +
        "\nID already appears here: \n" + errorLink(originalSample.lineNumber,
                                                    ErrorLevel::INFO);
}

const std::string ErrorMessages::ERROR_SELF_PARENT(const Sample& sample) const {
    return errorMsg(sample.lineNumber, ErrorLevel::ERROR,
                    "Parent ID can not be itself");
}




////////////////////////////////////////////////////////////////////////////////
//             NEUROLUCIDA
////////////////////////////////////////////////////////////////////////////////
const std::string ErrorMessages::ERROR_SOMA_ALREADY_DEFINED(int lineNumber) const {
    return errorMsg(lineNumber, ErrorLevel::ERROR,
                    "A soma is already defined");
}


const std::string ErrorMessages::ERROR_PARSING_POINT(int lineNumber,
                                      const std::string& point) const {
    return errorMsg(lineNumber, ErrorLevel::ERROR,
                    "Error converting: \"" + point + "\" to float");
}

const std::string ErrorMessages::ERROR_UNKNOWN_TOKEN(int lineNumber,
                                      const std::string& token) const {
    return errorMsg(lineNumber, ErrorLevel::ERROR,
                    "Unexpected token: " + token);
}

const std::string ErrorMessages::ERROR_UNEXPECTED_TOKEN(int lineNumber,
                                         const std::string& expected,
                                         const std::string& got,
                                         const std::string& msg) const {
    return errorMsg(lineNumber, ErrorLevel::ERROR,
                    "Unexpected token\nExpected: " + expected
                    + " but got " + got + " " + msg);
}

const std::string ErrorMessages::ERROR_EOF_REACHED(int lineNumber) const {
    return errorMsg(lineNumber, ErrorLevel::ERROR,
                    "Can't iterate past the end");
}

const std::string ErrorMessages::ERROR_EOF_IN_NEURITE(int lineNumber) const {
    return errorMsg(lineNumber, ErrorLevel::ERROR,
                    "Hit end of file while consuming a neurite");
}

const std::string ErrorMessages::ERROR_EOF_UNBALANCED_PARENS(int lineNumber) const {
    return errorMsg(lineNumber, ErrorLevel::ERROR,
                    "Hit end of file before balanced parens");
}

const std::string ErrorMessages::ERROR_UNCOMPATIBLE_FLAGS(morphio::Option flag1,
                                           morphio::Option flag2) const {
    return errorMsg(0, ErrorLevel::ERROR,
                    "Modifiers: " + std::to_string(flag1) + " and : " +
                    std::to_string(flag2) + " are incompatible");
}



////////////////////////////////////////////////////////////////////////////////
//              WRITERS
////////////////////////////////////////////////////////////////////////////////

const std::string ErrorMessages::ERROR_WRONG_EXTENSION(const std::string filename) const {
    return "Filename: " + filename +
        " must have one of the following extensions: swc, asc or h5";
};


std::string ErrorMessages::ERROR_WRITE_NO_SOMA() const {
    return "Cannot write file without a soma";
}

std::string ErrorMessages::ERROR_VECTOR_LENGTH_MISMATCH(const std::string& vec1, int length1,
                                         const std::string& vec2, int length2) const {
    std::string msg("Vector length mismatch: \nLength "+ vec1 + ": " +
                    std::to_string(length1) +
                    "\nLength "+ vec2 + ": " + std::to_string(length2));
    if(length1 == 0 || length2 == 0)
        msg += "\nTip: Did you forget to fill vector: " + (length1 == 0 ? vec1 : vec2) + " ?";

    return msg;
}

////////////////////////////////////////////////////////////////////////////////
//              WARNINGS
////////////////////////////////////////////////////////////////////////////////
std::string ErrorMessages::WARNING_NO_SOMA_FOUND() const {
    return errorMsg(0, ErrorLevel::WARNING,
                    "No soma found in file");
}

std::string ErrorMessages::WARNING_DISCONNECTED_NEURITE(const Sample& sample) const {
    return errorMsg(sample.lineNumber, ErrorLevel::WARNING,
                    "Found a disconnected neurite.\n"
                    "Neurites are not supposed to have parentId: -1\n"
                    "(although this is normal if this neuron has no soma)");
}

std::string ErrorMessages::WARNING_WRONG_DUPLICATE(std::shared_ptr<morphio::mut::Section> current,
                                    std::shared_ptr<morphio::mut::Section> parent) const {

    std::string msg("While appending section: "+std::to_string(current->id()) +
                    " to parent: "+std::to_string(parent->id()));

    if(parent->points().empty())
        return errorMsg(0, ErrorLevel::WARNING, msg +
                        "\nThe parent section is empty.");

    if(current->points().empty())
        return errorMsg(0, ErrorLevel::WARNING, msg +
                        "\nThe current section has no points. It should at least contains " +
                        "parent section last point");

    auto p0 = parent->points()[parent->points().size()-1];
    auto p1 = current->points()[0];
    auto d0 = parent->diameters()[parent->diameters().size()-1];
    auto d1 = current->diameters()[0];

    return errorMsg(0, ErrorLevel::WARNING, msg +
                    "\nThe section first point " +
                    "should be parent section last point: " +
                    "\n        : X Y Z Diameter" +
                    "\nparent  :["+std::to_string(p0[0]) + ", "+std::to_string(p0[0]) + ", "+std::to_string(p0[0]) + ", "+std::to_string(d0) + "]" +
                    "\ncurrent :["+std::to_string(p1[0]) + ", "+std::to_string(p1[0]) + ", "+std::to_string(p1[0]) + ", "+std::to_string(d1) + "]\n");

}


const std::string ErrorMessages::WARNING_ONLY_CHILD(const DebugInfo& info, int parentId, int childId) const {
    int parentLine = info.getLineNumber(parentId);
    int childLine =  info.getLineNumber(childId);
    std::string parentMsg, childMsg;
    if(parentLine > -1 && childLine > -1) {
        parentMsg =  " starting at:\n" + errorLink(parentLine, ErrorLevel::INFO) + "\n";
        childMsg = " starting at:\n" + errorLink(childLine, ErrorLevel::WARNING) + "\n";
    }

    return "\nSection: " + std::to_string(childId) + childMsg + " is the only child of " +
        "section: " + std::to_string(parentId) + parentMsg +
        "\nIt will be merged with the parent section";
}



} // namespace plugin

} // namespace morphio