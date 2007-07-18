#include "trimesh.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
using namespace std;


// Constructor
trimesh_t::trimesh_t() {
	int i;
	for (i=0; i<3; i++) {
		voxel_table[i] = NULL;
		verticies_table[i] = NULL;
		voxel_centers_table[i] = NULL;
	}
	num_triangles = 0;
}


// Set up the tables used to populate a trimesh
void trimesh_t::initialize_tables() {
	int i,j;
	for (i=0; i<3; i++) {
		voxel_table[i] = new char[nx*ny];
		verticies_table[i] = new vector_t*[(nx-1)*(ny-1)];
		voxel_centers_table[i] = new vector_t*[nx*ny];
		for (j=0; j< (nx-1)*(ny-1); j++) {
			verticies_table[i][j] = NULL;
		}
		for (j=0; j< nx*ny; j++) {
			voxel_centers_table[i][j] = NULL;
		}	
	}		
}

void trimesh_t::fill_voxels(interval_t X, interval_t Y, int index) {
	// Z interval increments on each call
	interval_t Z;
	Z.set_real_interval(zhorizon, zhorizon + zstep);
	zhorizon += zstep;
	// Setup slice evaluation region
	space_interval_t slice(X,Y,Z);
	// Evaluate single slice of voxel grid
	octree->eval_on_grid(slice, nx, ny, 1, voxel_table[index]);
}


// Populate a trimesh using an evaluated octree on a given space interval,
// with a rectangular lattice using an underlying voxel grid with
// nx, ny, and nz elements in each direction
void trimesh_t::populate(octree_t* octree, space_interval_t* region, int nx, int ny, int nz) {
	int ix, iy, iz, i, index;
	
	//cout << "nx: " << nx << "ny: " << ny << " nz: " << nz << "\n";

	// Store pointer to octree to evaluate
	this->octree = octree;

	// Set up class variables used for population
	this->nx = nx;
	this->ny = ny;
	this->nz = nz;
	initialize_tables();

	// Set up X and Y evaluation intervals
	interval_t X(region->get_var_value(0));
	interval_t Y(region->get_var_value(1));
	interval_t Z(region->get_var_value(1));
	
	// Set up voxel grid step size
	xstep = X.get_length()/(float)nx;
	ystep = Y.get_length()/(float)ny;
	zstep = Z.get_length()/(float)nz;
	xstart = X.get_lower();
	ystart = Y.get_lower();
	zstart = Z.get_lower();

	// Initialize the Z horizon for the voxel slice evaluation
	zhorizon = Z.get_lower();

	// Initialize the voxel grid with three slices
	for(i=0; i<3; i++) {
		fill_voxels(X,Y,i);
	}

	// Iterate over every voxel except the outside edge, Z direction first
	z = Z.get_lower() + zstep/2.0f;
	for(iz = 1; iz<(nz-1); iz++) {
		z+= zstep;

		y = Y.get_lower() + ystep/2.0f;
		for(iy=1; iy<(ny-1); iy++) {
			y+= ystep;
			
			x = X.get_lower() + xstep/2.0f; 
			for(ix=1; ix<(nx-1); ix++) {
				x+= xstep;
				
				index = ix + nx*iy;		// Linear array index for this voxel

				// For each voxel:

				// If true here true here
				if (voxel_table[1][index] == 1) {
					// But false to the ... then build two triangles
					// ...left (-X)
					if (voxel_table[1][index - 1] == 0) 
						triangulate_face(1, index, ix, iy, iz, 1, index-1, ix-1, iy, iz);							
					// ...right (+X)
					if (voxel_table[1][index + 1] == 0) 
						triangulate_face(1, index, ix, iy, iz, 1, index+1, ix+1, iy, iz);
					// ... backward (-Y)
					if (voxel_table[1][index - nx] == 0)
						triangulate_face(1, index, ix, iy, iz, 1, index-nx, ix, iy-1, iz);
					// ... forward (+Y)
					if (voxel_table[1][index + ny] == 0)
						triangulate_face(1, index, ix, iy, iz, 1, index+nx, ix, iy+1, iz);
					// ... down (-Z)
					if (voxel_table[0][index] == 0)
						triangulate_face(1, index, ix, iy, iz, 0, index, ix, iy, iz-1);
					// ... up (+Z)
					if (voxel_table[2][index] == 0)
						triangulate_face(1, index, ix, iy, iz, 2, index, ix, iy, iz+1);
				}
			}
		}
		
		// If the loop will run again
		if ((iz+1) < (nz-1)) {

			// Roll through voxel grid
			delete voxel_table[0];
			voxel_table[0] = voxel_table[1];
			voxel_table[1] = voxel_table[2];
			voxel_table[2] = new char[nx*ny];
			fill_voxels(X,Y,2);

			delete verticies_table[0];
			verticies_table[0] = verticies_table[1];
			verticies_table[1] = verticies_table[2];
			verticies_table[2] = new vector_t*[(nx-1)*(ny-1)];
			
			for (i=0; i< (nx-1)*(ny-1); i++) {
				verticies_table[2][i] = NULL;
			}
		
			delete voxel_centers_table[0];
			voxel_centers_table[0] = voxel_centers_table[1];
			voxel_centers_table[1] = voxel_centers_table[2];
			voxel_centers_table[2] = new vector_t*[nx*ny];
			for (i=0; i< nx*ny; i++) {
				voxel_centers_table[2][i] = NULL;
			}	
			//cout << "DID ROLLOVER\n";
		}
		else {
			// All done - Free dynamic memory used for pointer tables		
			for(i=0; i<3; i++) {
				delete verticies_table[i];
				delete voxel_centers_table[i];
			}
		}
	
	}
}


// Builds two triangles on a rectangular face dividing a voxel inside the
// object from a voxel outside the object
void trimesh_t::triangulate_face(int in_slice, int in_index,
							   int in_ix, int in_iy, int in_iz,
							   int out_slice, int out_index,
							   int out_ix, int out_iy, int out_iz) {
	float x, y, z;
	int xv, yv, zv;
	int index;
	vector_t *quad[4];
	vector_t normal;
	trimesh_node_t* node;
	int edge_ix, edge_iy, edge_iz, dir, i, edge_slice, t;


	int a[2][4];
	int b[2][4];

	// Order of quadrilateral traversal
	// 0 is a-first, 1 is b-first
	a[0][0] = 0; b[0][0] = 0;
	a[0][1] = 1; b[0][1] = 0;
	a[0][2] = 1; b[0][2] = 1;
	a[0][3] = 0; b[0][3] = 1;

	a[1][0] = 0; b[1][0] = 0;
	a[1][1] = 0; b[1][1] = 1;
	a[1][2] = 1; b[1][2] = 1;
	a[1][3] = 1; b[1][3] = 0;


	// Add voxel centers to shared tables
	if (voxel_centers_table[in_slice][in_index] == NULL) {
		x = xstart + in_ix*xstep;
		y = ystart + in_iy*ystep;
		z = zstart + in_iz*zstep;
		voxel_centers_table[in_slice][in_index] = new vector_t(x, y, z);
	}
	if (voxel_centers_table[out_slice][out_index] == NULL) {
		x = xstart + out_ix*xstep;
		y = ystart + out_iy*ystep;
		z = zstart + out_iz*zstep;
		voxel_centers_table[out_slice][out_index] = new vector_t(x, y, z);
	}

	// Add verticies to shared table, and record pointers to those 
	// verticies for a quadrilateral (rectangle) counterclockwise
	// around the normal vector to the face.


	// If face is normal to the X axis
	if (in_ix != out_ix) {
		// Select min(in_ix,out_ix) --> This is vertex table edge index 
		if (in_ix < out_ix) {
			edge_ix = in_ix;
			dir = 0;  // from 0,0, pos y first points out for in smaller
			normal.set(1.0, 0.0, 0.0);
		}
		else { 
			edge_ix = out_ix;
			dir = 1;
			normal.set(-1.0,0.0,0.0);
		}
		// Add four verticies in a rectangle in the YZ plane
		for(i=0; i<4; i++) {
			yv = a[dir][i];  
			zv = b[dir][i];

			index = edge_ix + (nx-1)*(in_iy + yv-1);			
			if (verticies_table[zv][index] == NULL) {
				x = xstart + (xstep * (edge_ix + 0.5f)) + 10.0f;     
				y = ystart + ystep * (in_iy - 0.5f + yv) + 10.0f;   
				z = zstart + zstep * (in_iz - 0.5f + zv) + 10.0f;
				verticies_table[zv][index] = new vector_t(x,y,z);
				verticies.push_back(verticies_table[zv][index]);
				vertex_inside_point.push_back(voxel_centers_table[in_slice][in_index]);
				vertex_outside_point.push_back(voxel_centers_table[out_slice][out_index]);
			}		
			quad[i] = verticies_table[zv][index];
		}
	}

	// If face is normal to the Y axis
	if (in_iy != out_iy) {
		// Select min(in_iy,ouy_iy) --> This is vertex table edge index 
		if (in_iy < out_iy) {
			edge_iy = in_iy;
			dir = 1;	 
			normal.set(0.0f, 1.0f, 0.0f);
		}
		else { 
			edge_iy = out_iy;
			dir = 0;	// from 0,0, pos X first is in larger
			normal.set(0.0f, -1.0f, 0.0f);
		}
		// Add four verticies in a rectangle in the XZ plane
		for(i=0; i<4; i++) {
			xv = a[dir][i];  
			zv = b[dir][i];

			//cout << "index calc: in_ix:" << in_ix << "xv: " << xv << " nx: " << nx << "edge_iy: " << edge_iy << "\n";
			index = in_ix-1 + xv + (nx-1)*edge_iy;			
			if (verticies_table[zv][index] == NULL) {
				x = xstart + (xstep * (in_ix - 0.5f + xv)) + 10.0f;     
				y = ystart + ystep * (edge_iy + 0.5f) + 10.0f;   
				z = zstart + zstep * (in_iz - 0.5f + zv) + 10.0f;
				verticies_table[zv][index] = new vector_t(x,y,z);
				//cout << "created new entry at " << verticies_table[zv][index] << "\n";
				verticies.push_back(verticies_table[zv][index]);
				vertex_inside_point.push_back(voxel_centers_table[in_slice][in_index]);
				vertex_outside_point.push_back(voxel_centers_table[out_slice][out_index]);
			}				
			quad[i] = verticies_table[zv][index];
			//cout << "quad["<< i << "]=verticies_table["<<zv<<"]["<<index<<"]\n";

		}	
	}

	// If face is normal to the Z axis
	if (in_iz != out_iz) {
		// Select min(in_iz,in_iz) --> This is vertex table edge index 
		if (in_iz < out_iz) {
			edge_iz = in_iz;
			edge_slice = in_slice;
			dir = 0;	// from 0,0, pos x first is in smaller
			normal.set(0.0f, 0.0f, 1.0f);
		}
		else { 
			edge_iz = out_iz;
			edge_slice = out_slice;
			dir = 1;
			normal.set(0.0f, 0.0f, -1.0f);
		}

		// Add four verticies in a rectangle in the XY plane
		for(i=0; i<4; i++) {
			xv = a[dir][i];  
			yv = b[dir][i];

			index = in_ix-1 + xv + (nx-1)*(in_iy + yv-1);			
			if (verticies_table[edge_slice][index] == NULL) {
				x = xstart + xstep * (in_ix - 0.5f + xv) + 10.0f;   
				y = zstart + ystep * (in_iy - 0.5f + yv) + 10.0f;
				z = zstart + (zstep * (edge_iz + 0.5f)) + 10.0f;      
				verticies_table[edge_slice][index] = new vector_t(x,y,z);
				verticies.push_back(verticies_table[edge_slice][index]);
				vertex_inside_point.push_back(voxel_centers_table[in_slice][in_index]);
				vertex_outside_point.push_back(voxel_centers_table[out_slice][out_index]);
			}				
			quad[i] = verticies_table[edge_slice][index];
		}
	}

	// Write two triangles to tile quadrilateral	
	//cout << "New quad.\n";
	for (t=0; t<4; t+=2) {
		node = new trimesh_node_t();
		for(i=0; i<3; i++) {
			//cout << i << " " << ((i+t) % 4) << " ";
			node->verticies[i] = quad[((i+t) % 4)];
			//cout << (node->verticies[i]) << "\n";
		}
		node->normal = normal;
		node->interior_point = voxel_centers_table[in_slice][in_index];
		node->exterior_point = voxel_centers_table[out_slice][out_index];
	
		// Add to list
		//cout << "added.\n";
		triangles.push_back(node);
		num_triangles++;
	}
}


// Display a trimesh (text)
ostream& operator<<(ostream &s, trimesh_t &v) {   
   list<trimesh_node_t*>::iterator triangle_iterator;
   list<vector_t*>::iterator vertex_iterator;

   s << "Triangles:\n";

   for (triangle_iterator = v.triangles.begin(); triangle_iterator != v.triangles.end(); triangle_iterator++) {
     s << **triangle_iterator << "\n";
   }
   
   s << "Verticies:\n";
   for (vertex_iterator = v.verticies.begin(); vertex_iterator != v.verticies.end(); vertex_iterator++) {
	   s << *vertex_iterator << " ";
	   s << **vertex_iterator << "\n";
   }

   return(s);

}


ostream& operator<<(ostream &s, const trimesh_node_t &v) {
	int i;

	s << "Verticies: ";
	for (i=0; i<3; i++) {
		//s << (v.verticies[i]) << " ";
    	s << *(v.verticies[i]) << " ";
	}
	s << "Normal: " << v.normal << " ";
//	s << "Interior Point: " << *(v.interior_point) << " ";
//	s << "Exterior Point: " << *(v.exterior_point) << " ";
	s << "\n";

	return(s);
}





void trimesh_t::write_stl(string filename) {
	
// Binary STL file format is described in
// Automated Fabrication, by Marshall Burns, Ch.6
// and at http://www.ennex.com/~fabbers/StL.asp
	 
	fstream stl_file (filename.c_str(), ios::binary | ios::out | ios::trunc);
	char header[80] = "Created by geomeval 0.1";
    list<trimesh_node_t*>::iterator triangle_iterator;
	stl_facet_t stl_facet;
	unsigned int num_facets = (unsigned int)num_triangles;  
	unsigned short attr_byte_count = 0;

	if (stl_file.fail()) {
		cout << "Error opening STL output file.\n";
		return;
	}
	else {
//		cout << "writing STL file.\n";
		stl_file.write(header, 80);
		stl_file.write((char*)&num_facets, 4);

//		stl_facet.abc0 = 0;
//		stl_facet.abc1 = 0;

		//cout << "sizeof char " << sizeof(char) << "\n";
		//cout << "sizeof vector_t " << sizeof(vector_t) << "\n";
		//cout << "sizeof float " << sizeof(float) << "\n";


		for (triangle_iterator = triangles.begin(); triangle_iterator != triangles.end(); triangle_iterator++) {
			stl_facet.normal = (*triangle_iterator)->normal;
			stl_facet.vertex_1 = *((*triangle_iterator)->verticies[0]);
			stl_facet.vertex_2 = *((*triangle_iterator)->verticies[1]);
			stl_facet.vertex_3 = *((*triangle_iterator)->verticies[2]);

			stl_file.write((char*)&stl_facet, sizeof(stl_facet_t));
			stl_file.write((char*)&attr_byte_count, sizeof(unsigned short));	
//			cout << "wrote facet size " << sizeof(stl_facet_t) + sizeof(unsigned short) << "\n";
			
		}
		
		stl_file.close();
//		cout << "closed file.\n";
		
	}

}




// procedure:  walk octree, generating triangulated mesh.  need to mark
// triangle neightbors so that tree is walkable for corner finding

// generate triangles from voxels on sections of the evaluated voxel
// grid.  This makes it easy to mark most triangles neigbors...just look over.
// Finding boundary triangle neighbors could be tough w/o storage.

// Need to search?  search for triangles without full complement of neighbors?
// Or, do the object one layer at a time, always keeping two full layers, deleting
// a layer just as the third is done.  This means neighboring triangles are
// automatically recorded.

// Identifying non-convex connected regions presents the standard connected
// components problem: initiallly seperate regions may become the joined on a
// new layer.  you can join connected regions by starting them out seperate and
// then merging them...you can do all the triangle connections because the ragged
// edge is fully documented by the layers stored in memory

// writing STL or rendering OpenGL is easy: you iterate through the list

// when writing xy slide, could put a code "2" in ints that are guaranteed not to be edges,
// by being in interior of all 1's and all 0's regions, so the program does not even
// have to check


