# -*- coding: utf-8 -*-
# =====================================================================================================================
# These bindings were automatically generated by cyWrap. Please do dot modify.
# Additional functionality shall be implemented in sub-classes.
#
__copyright__ = "Copyright 2017 EPFL BBP-project"
# =====================================================================================================================
from cython.operator cimport dereference as deref
from libcpp cimport bool
cimport std
cimport morpho_h5_v1
cimport boost_numeric_ublas
cimport hadoken_geometry
from statics.morpho_serialization_format cimport serialization_format


# ======================================================================================================================
cdef extern from "morpho/morpho_types.hpp" namespace "morpho":
# ----------------------------------------------------------------------------------------------------------------------
    ctypedef boost_numeric_ublas.matrix[double] mat_points
    ctypedef boost_numeric_ublas.vector_range[mat_points] vec_double_range
    ctypedef boost_numeric_ublas.vector[double] vec_double
    ctypedef boost_numeric_ublas.matrix_range[vec_double] mat_range_points
    ctypedef boost_numeric_ublas.matrix[int] mat_index

    ctypedef hadoken_geometry.point3d point
    ctypedef hadoken_geometry.vector3d vector
    ctypedef hadoken_geometry.linestring3d linestring
    ctypedef hadoken_geometry.sphere3d sphere
    ctypedef hadoken_geometry.circle3d circle
    ctypedef hadoken_geometry.cone3d cone
    ctypedef std.vector[circle] circle_pipe
    ctypedef hadoken_geometry.box3d box

# ======================================================================================================================
cdef extern from "morpho/morpho_mesher.hpp" namespace "morpho":
# ----------------------------------------------------------------------------------------------------------------------

    ###### Cybinding for class morpho_mesher ######
    cdef cppclass morpho_mesher:
        enum mesh_tag:
            pass

        morpho_mesher(const morpho_mesher&) #copy ctor
        morpho_mesher(std.shared_ptr[morpho_tree], std.string)
        void set_mesh_tag(morpho_mesher.mesh_tag, bool)
        void set_error_bound(double)
        void set_face_size(double)
        void execute()


# ======================================================================================================================
cdef extern from "morpho/morpho_transform.hpp" namespace "morpho":
# ----------------------------------------------------------------------------------------------------------------------

    ###### Cybinding for class morpho_operation ######
    cdef cppclass morpho_operation:
        morpho_tree apply(morpho_tree)
        std.string name()

    # morpho_transform function
    morpho_tree morpho_transform(const morpho_tree &, std.vector[std.shared_ptr[morpho_operation]])


# ======================================================================================================================
cdef extern from "morpho/morpho_transform_filters.hpp" namespace "morpho::transpose_operation":
# ----------------------------------------------------------------------------------------------------------------------
    # cython hack for having integer template
    ctypedef int three_t "3"
    ctypedef int four_t "4"
    ctypedef std.array[double, three_t] vector3d
    ctypedef std.array[double, four_t] quaternion3d


# ======================================================================================================================
cdef extern from "morpho/morpho_transform_filters.hpp" namespace "morpho":
# ----------------------------------------------------------------------------------------------------------------------

    ###### Cybinding for class delete_duplicate_point_operation ######
    cdef cppclass delete_duplicate_point_operation(morpho_operation):
        delete_duplicate_point_operation()
        delete_duplicate_point_operation(const delete_duplicate_point_operation&)

    ###### Cybinding for class duplicate_first_point_operation ######
    cdef cppclass duplicate_first_point_operation(morpho_operation):
        duplicate_first_point_operation()
        duplicate_first_point_operation(const duplicate_first_point_operation&)

    ###### Cybinding for class soma_sphere_operation ######
    cdef cppclass soma_sphere_operation(morpho_operation):
        soma_sphere_operation()
        soma_sphere_operation(soma_sphere_operation&)

    ###### Cybinding for class simplify_section_extreme_operation ######
    cdef cppclass simplify_section_extreme_operation(morpho_operation):
        simplify_section_extreme_operation()
        simplify_section_extreme_operation(simplify_section_extreme_operation&)

    ###### Cybinding for class transpose_operation ######
    cdef cppclass transpose_operation(morpho_operation):
        transpose_operation(const vector3d&, const quaternion3d&)
        transpose_operation(transpose_operation&)


# ======================================================================================================================
cdef extern from "morpho/morpho_spatial.hpp" namespace "morpho":
# ----------------------------------------------------------------------------------------------------------------------

    ###### Cybinding for class spatial_index ######
    cdef cppclass spatial_index:
        spatial_index()
        #spatial_index(const spatial_index&)
        void add_morpho_tree(std.shared_ptr[morpho_tree])
        bool is_within(point)


# ======================================================================================================================
cdef extern from "morpho/morpho_tree.hpp" namespace "morpho::cell_family":
# ----------------------------------------------------------------------------------------------------------------------
    cdef enum cell_family "morpho::cell_family":
        NEURON
        GLIA


# ======================================================================================================================
cdef extern from "morpho/morpho_tree.hpp" namespace "morpho":
# ----------------------------------------------------------------------------------------------------------------------
    # Enum classes have their members inner -> Declared under /statics
    cdef cppclass morpho_node_type:
        pass

    cdef cppclass neuron_struct_type:
        pass

    cdef cppclass glia_struct_type:
        pass

    ###### Cybinding for class morpho_node ######
    cdef cppclass morpho_node:
        morpho_node()
        morpho_node(const morpho_node)
        box get_bounding_box()
        bool is_of_type(morpho_node_type)

    ###### Cybinding for class neuron_node_3d ######
    cdef cppclass neuron_node_3d(morpho_node):
        neuron_node_3d(neuron_struct_type)
        neuron_node_3d(const neuron_node_3d)
        neuron_struct_type get_section_type()
        #bool is_of_type(morpho_node_type)

    ###### Cybinding for class neuron_section ######
    cdef cppclass neuron_section(neuron_node_3d):
        neuron_section(neuron_struct_type, std.vector[point], std.vector[double])
        neuron_section(neuron_section)
        #bool is_of_type(morpho_node_type)
        std.size_t get_number_points()
        std.vector[point] get_points()
        std.vector[double] get_radius()
        cone get_segment(std.size_t)
        #box get_bounding_box()
        box get_segment_bounding_box(std.size_t)
        sphere get_junction(std.size_t)
        box get_junction_sphere_bounding_box(std.size_t)
        linestring get_linestring()
        circle_pipe get_circle_pipe()

    ###### Cybinding for class neuron_soma ######
    cdef cppclass neuron_soma(neuron_node_3d):
        neuron_soma(std.vector[point] &&)
        neuron_soma(point, double)
        #bool is_of_type(morpho_node_type)
        sphere get_sphere()
        #box get_bounding_box()
        std.vector[point] get_line_loop()

    ###### Cybinding for class morpho_tree ######
    cdef cppclass morpho_tree:
        morpho_tree()
        morpho_tree(morpho_tree)
        morpho_tree operator=(morpho_tree)
        morpho_tree operator=(morpho_tree)
        box get_bounding_box()
        std.size_t get_tree_size()
        void swap(morpho_tree)
        int add_node(int, std.shared_ptr[morpho_node]) except +
        int copy_node(morpho_tree, int, int) except +
        morpho_node get_node(int)
        int get_parent(int) except +
        std.vector[int] get_children(int)
        std.vector[morpho_node*] get_all_nodes()
        void set_cell_type(cell_family)
        cell_family get_cell_type()


# ======================================================================================================================
cdef extern from "morpho/morpho_tree_algorithm.hpp" namespace "morpho":
# ----------------------------------------------------------------------------------------------------------------------
    std.vector[int] find_neuron_nodes(const morpho_tree&, neuron_struct_type)
    neuron_soma* find_neuron_soma(const morpho_tree&)


# Specialize std::move (only way of using it in cython)
cdef extern from "<utility>" namespace "std":
    cdef std.vector[point] move_PointVector "std::move" (std.vector[point])

cdef extern from "<utility>" namespace "std":
    cdef std.vector[double] move_DoubleVec "std::move" (std.vector[double])


# ======================================================================================================================
cdef extern from "morpho/morpho_serialization.hpp" namespace "morpho":
# ----------------------------------------------------------------------------------------------------------------------
    cdef std.string serialize(const morpho_tree&, const serialization_format&)
    cdef morpho_tree deserialize(const std.string&, const serialization_format&)
