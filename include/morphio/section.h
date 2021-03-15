#pragma once

#include <memory>  // std::shared_ptr

#include <morphio/properties.h>
#include <morphio/section_base.h>
#include <morphio/section_iterators.hpp>
#include <morphio/types.h>

namespace morphio {
/**
 * A class to represent a morphological section.
 *
 * A Section is an unbranched piece of a morphological skeleton.
 * This class provides functions to query information about the sample points
 * that compose the section and functions to obtain the parent and children
 * sections.
 *
 * The cell soma is also considered a section, but some functions have
 * special meaning for it.
 *
 * Sections cannot be directly created, but are returned by several
 * morphio::Morphology and morphio::Section methods.
 *
 * This is a lightweight object with STL container style thread safety.
 * It is also safe to use a section after the morphology from where it comes
 * has been deallocated. The morphological data will be kept as long as there
 * is a Section referring to it.
 */

class Morphology;
template <typename Family>
using upstream_iterator = upstream_iterator_t<Section<Family>>;
template <typename Family>
using breadth_iterator = breadth_iterator_t<Section<Family>, Morphology>;
template <typename Family>
using depth_iterator = depth_iterator_t<Section<Family>, Morphology>;

/*
template <typename Family>
class Section {
public:
 Section<Family>() {};
 typename Family::Type type;

};
*/
template <typename Family>
class Section: public SectionBase<Section<Family>>
{
    using SectionId = Property::Section;
    using PointAttribute = Property::Point;

  public:
    //using Type = SectionType;
    using Type = typename Family::Type;

    /**
       Depth first search iterator
    **/
    depth_iterator<Family> depth_begin() const;
    depth_iterator<Family> depth_end() const;

    /**
       Breadth first search iterator
    **/
    breadth_iterator<Family> breadth_begin() const;
    breadth_iterator<Family> breadth_end() const;

    /**
       Upstream first search iterator
    **/
    upstream_iterator<Family> upstream_begin() const;
    upstream_iterator<Family> upstream_end() const;

    /**
     * Return a view
    (https://github.com/isocpp/CppCoreGuidelines/blob/master/docs/gsl-intro.md#gslspan-what-is-gslspan-and-what-is-it-for)
     to this section's point coordinates
    **/
    range<const Point> points() const;

    /**
     * Return a view
    (https://github.com/isocpp/CppCoreGuidelines/blob/master/docs/gsl-intro.md#gslspan-what-is-gslspan-and-what-is-it-for)
     to this section's point diameters
    **/
    range<const floatType> diameters() const;

    /**
     * Return a view
     (https://github.com/isocpp/CppCoreGuidelines/blob/master/docs/gsl-intro.md#gslspan-what-is-gslspan-and-what-is-it-for)
     to this section's point perimeters
     **/
    range<const floatType> perimeters() const;

    /**
     * Return the morphological type of this section (dendrite, axon, ...)
     */
    Type type() const;
    friend class mut::Section;

    template<typename Node, typename CRTP, typename Mut>
    friend class TTree;

    friend class SectionBase<Section>;

  protected:
    Section<Family>(uint32_t id_, const std::shared_ptr<Property::Properties>& properties)
        : SectionBase<Section<Family>>(id_, properties) {}
};

// explicit instanciation
template class SectionBase<Section<CellFamily::NEURON>>;
template class SectionBase<Section<CellFamily::GLIA>>;

}  // namespace morphio

template <typename Family>
std::ostream& operator<<(std::ostream& os, const morphio::Section<Family>& section);
std::ostream& operator<<(std::ostream& os, const morphio::range<const morphio::Point>& points);
