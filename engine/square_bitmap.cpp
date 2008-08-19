#include "square_bitmap.h"
using namespace std;

square_bitmap_t::square_bitmap_t() {
	s = 0;
}


square_bitmap_t::square_bitmap_t(int value, int size) {
		s = size;		
		int numel = s*s;
		int numelv = (s+1)*(s+1);
		data = new int[numel];
		verticies = new vertex_t*[numelv];
		points = new vector_t*[numel];
		for (int i=0; i < numel; i++) {
			data[i] = value;
			points[i] = NULL;
		}
		for (int i=0; i<numelv; i++) {
			verticies[i] = NULL;
		}
   }

 square_bitmap_t::~square_bitmap_t() {
		delete[] data;
		delete[] verticies;
	    delete[] points;
   }

   // combines four equal-sized square bitmaps into one new one by copying
   // if one vertex or point is null and the other isn't at the boundaries, uses the non-null
 square_bitmap_t::square_bitmap_t(square_bitmap_t *bmp0, square_bitmap_t *bmp1, square_bitmap_t *bmp2, square_bitmap_t *bmp3) {
		int os = bmp0->s;
		s = 2*os;
		int numel = s*s;
		int numelv = (s+1)*(s+1);
		
		data = new int[numel];
		verticies = new vertex_t*[numelv];
		points = new vector_t*[numel];
		
		int ptr = 0;
		int ptr0 = 0;
		int ptr1 = 0;
		int ptr2 = 0;
		int ptr3 = 0;

		// Copy from old bitmaps, arranged in raster order by blocks
		for (int i=0; i<os; i++) {
			 for (int j=0; j<os; j++) {
				points[ptr] = bmp0->points[ptr0];
				data[ptr++] = bmp0->data[ptr0++];
			
			 }
			 for (int j=0; j<os; j++) {
				points[ptr] = bmp1->points[ptr1];
				data[ptr++] = bmp1->data[ptr1++];
			 }
		}
		for (int i=0; i<os; i++) {
			 for (int j=0; j<os; j++) {
				points[ptr] = bmp2->points[ptr2];
				data[ptr++] = bmp2->data[ptr2++];
			 }
			 for (int j=0; j<os; j++) {
				points[ptr] = bmp3->points[ptr3];
				data[ptr++] = bmp3->data[ptr3++];
			 }
		}







		// Copy verticies
		// At this point, the verticies are already knitted, so we can just throw out one side or the other arbitarily at seams

		ptr = 0;
		ptr0 = 0;
		ptr1 = 0;
		ptr2 = 0;
		ptr3 = 0;
		for (int i=0; i<os+1; i++) {
			 for (int j=0; j<os+1; j++) {
				verticies[ptr++] = bmp0->verticies[ptr0++];
			
			 }
			 if (bmp0->verticies[ptr0-1] != bmp1->verticies[ptr1]) {
				 cout << " oops!";
			 }

			 ptr1++;  // skip first col of 1 (dupe of last col of 0)
			 for (int j=0; j<os; j++) {
				verticies[ptr++] = bmp1->verticies[ptr1++];
			 }
			 
		}
		ptr2 = os+1;	// skip first row of 2 and 3 (dupe of last row of 0 and 1)
		ptr3 = os+1;
		for (int i=0; i<os; i++) {
			 for (int j=0; j<os+1; j++) {
				verticies[ptr++] = bmp2->verticies[ptr2++];
			 }

			 if (bmp2->verticies[ptr2-1] != bmp3->verticies[ptr3]) {
				 cout << " oops!";
			 }

			 ptr3++;  // skip first col of 2 (dupe of last col of 2)
			 for (int j=0; j<os; j++) {
				verticies[ptr++] = bmp3->verticies[ptr3++];
			 }
		}

   }


   int square_bitmap_t::get_data(int row, int col) {
		return(data[col + row*s]);
   }

   void square_bitmap_t::set_data(int value, int row, int col) {
		data[col + row*s] = value;
   }


   vertex_t* square_bitmap_t::get_vertex(int row, int col, int drow, int dcol) {
		return(verticies[col + dcol + (row+drow)*(s+1)]);
   }

   void square_bitmap_t::set_vertex(vertex_t *vertex, int row, int col, int drow, int dcol) {
		verticies[col + dcol + (row+drow)*(s+1)] = vertex;
   }

   vector_t* square_bitmap_t::get_point(int row, int col) {
		return(points[col + row*s]);
   }

   void square_bitmap_t::set_point(vector_t *point, int row, int col) {
		points[col + row*s] = point;
   }

   int square_bitmap_t::side_length() {
	   return(s);
   }
