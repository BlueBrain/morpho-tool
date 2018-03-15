#pragma once

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <ostream>

#include <functional>


#include <morphio/exceptions.h>
#include <morphio/properties.h>
#include <morphio/section.h>
#include <morphio/types.h>
#include <morphio/mut/soma.h>
#include <morphio/mut/iterators.h>

#include "../../../src/plugin/errorMessages.h"

namespace morphio
{
namespace mut
{


class Morphology
{
public:
    Morphology() : _soma(std::make_shared<Soma>(Soma())) , _counter(0) {}

    /**
       Build a mutable Morphology from an on-disk morphology
    **/
    Morphology(const morphio::URI& uri);

    /**
       Build a mutable Morphology from a read-only morphology
    **/
    Morphology(const morphio::Morphology& morphology);

    virtual ~Morphology();


    /**
       Returns all section ids at the tree root
    **/
    const std::vector<uint32_t>& rootSections() const;


    /**
       Returns the dictionary id -> Section for this tree
    **/
    const std::map<uint32_t, std::shared_ptr<Section>>& sections() const;

    /**
       Returns a shared pointer on the Soma

       Note: multiple morphologies can share the same Soma instance
    **/
    std::shared_ptr<Soma> soma();
    const std::shared_ptr<Soma> soma() const;

    /**
       Get the shared pointer for the given section identified

       Note: multiple morphologies can shared the same shared pointers.
    **/
    const std::shared_ptr<Section> section(uint32_t id) const;

    /**
       Get the parent ID

       Root sections return -1
    **/
    const int32_t parent(uint32_t id) const;

    /**
       Get children IDs
    **/
    const std::vector<uint32_t> children(uint32_t id) const;


    /**
       Return the data structure used to create read-only morphologies
    **/
    const Property::Properties buildReadOnly() const;



    /**
       Depth first iterator starting at a given section id

       If id == -1, the iteration will be successively performed starting
       at each root section
    **/
    depth_iterator depth_begin(uint32_t id = -1) const;
    depth_iterator depth_end() const;

    /**
       Breadth first iterator

       If id == -1, the iteration will be successively performed starting
       at each root section
    **/
    breadth_iterator breadth_begin(uint32_t id = -1) const;
    breadth_iterator breadth_end() const;

    /**
       Upstream first iterator

       If id == -1, the iteration will be successively performed starting
       at each root section
    **/
    upstream_iterator upstream_begin(uint32_t id = -1) const;
    upstream_iterator upstream_end() const;


    ////////////////////////////////////////////////////////////////////////////////
    //
    //   Tree manipulation methods
    //
    ////////////////////////////////////////////////////////////////////////////////

    /**
       Delete the given section

       Will silently fail if the section id is not part of the tree

       If recursive == true, all descendent sections will be deleted as well
       Else, children will be re-attached to their grand-parent
    **/
    void deleteSection(uint32_t id, bool recursive = true);

    /**
       Append the read-only Section to the given parentId (-1 appends to soma)

       If recursive == true, all descendent will be appended as well
    **/
    uint32_t appendSection(int32_t parentId, const morphio::Section&, bool recursive = true);

    /**
       Append the read-only Section to the given parentId (-1 appends to soma)

       If recursive == true, all descendent will be appended as well
    **/
    uint32_t appendSection(int32_t parentId, SectionType, const Property::PointLevel&);

    /**
       Iterate on all sections starting at startSection via a depth-first-search traversal
       and call the callback function fun.

       startSection specifies the starting section. if startSection == -1, the traversal
       will done on every neurite.
    **/
    // void traverse(std::function<void(Morphology& morphology, uint32_t sectionId)>,
    //               uint32_t startSection = -1);


    void write_h5(const std::string& filename);
    void write_asc(const std::string& filename);
    void write_swc(const std::string& filename);



private:
    friend class Section;

    morphio::plugin::ErrorMessages _err;

    uint32_t _register(std::shared_ptr<Section>);
    std::shared_ptr<Soma> _soma;
    std::vector<uint32_t> _rootSections;
    std::map<uint32_t, std::shared_ptr<Section>> _sections;

    uint32_t _counter;
    std::map<uint32_t, uint32_t> _parent;
    std::map<uint32_t, std::vector<uint32_t>> _children;
};

} // namespace mut
} // namespace morphio