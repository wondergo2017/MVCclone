#pragma once
#include <iostream>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>

#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <opencv2\opencv.hpp>
#include <fstream>
using namespace std;
using namespace cv;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_2<K> Vb;
typedef CGAL::Delaunay_mesh_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds> CDT;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT> Criteria;
typedef CGAL::Delaunay_mesher_2<CDT, Criteria> Mesher;

typedef CDT::Vertex_handle Vertex_handle;
typedef CDT::Point Pointcdt;
const Vec3f WHITE(1, 1, 1);
const Vec3f BLACK(0, 0, 0);
const Vec3f RED(0, 0, 1);
const Vec3f BLUE(1, 0, 0);
const Vec3f GREEN(0, 1, 0);
#define FLOODBORDER RED

#define FLOODED BLUE
#define FLOODNULL BLACK
#define FLOODOUT GREEN