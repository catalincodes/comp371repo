#include <iostream>
#include <vector>
#include "..\CImg\CImg.h"

int main()
{
	cimg_library::cimg::exception_mode(0);
	cimg_library::CImg<unsigned char> img;
	try {
		img = cimg_library::CImg<unsigned char>("org_image.bmp");
	}
	catch (cimg_library::CImgException &e) {
		std::cout << "Error = " << e.what() << std::endl;
	}

	unsigned char *ptr = img._data;
	
	cimg_library::CImg<unsigned char>::iterator it;

	std::cout << "Hello there!" << std::endl;
	return EXIT_SUCCESS;
}