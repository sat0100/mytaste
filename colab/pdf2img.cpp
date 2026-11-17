#include <iostream>
#include <qpdf/QPDF.hh>
#include <qpdf/QPDFWriter.hh>
#include <hpdf.h>
#include <hpdf_types.h>
#include <filesystem>

void error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data){
	printf("ERROR: error_no= %04X, detail_no=%d\n",
		(unsigned int) error_no, (int) detail_no);
	throw std::exception();
}

void show_help(){
	std::cout<<"Usage: pdf2img [img1] [img2] .... [output_filename]"
}
int main(int argc, char** argv){
	if(argc==1){
		printf("ERROR: insufficient arguments\n");
		show_help()
	}
	
	std::filesystem::path filepath;
	for(int i=1;i<argc-1;i++){
		filepath = std::string(argv[i]);
		if(!std::filesystem::exists(filepath)){
			printf("FILE:[ %s ]not found\n",argv[i]);
			return -1;
		}
	}
	filepath = std::string(argv[argc-1]);
	if(std::filesystem::exists(filepath)){
		printf("FILE:[ %s ] already exists\n",argv[argc-1]);
		return -2;
	}

	char *fname;
	HPDF_Doc pdf = HPDF_New(error_handler, NULL);
	if(!pdf) {
		printf("ERROR: Cannot create pdfdoc object\n");
		return 1;
	}
	try {
		for(int i=1;i<argc-1;i++){
			HPDF_Image image = HPDF_LoadJpegImageFromFile(pdf, argv[i]);
			if (!image) {
				printf("Error: Could not load image file.\n");
				HPDF_Free(pdf);
				return 1;
			}
		
			HPDF_REAL width = (HPDF_REAL)HPDF_Image_GetWidth(image);
			HPDF_REAL height = (HPDF_REAL)HPDF_Image_GetHeight(image);
			
			HPDF_Page page = HPDF_AddPage(pdf);
			HPDF_Page_SetWidth(page, width);
			HPDF_Page_SetHeight(page, height);
			
			HPDF_Page_DrawImage(page, image, 0, 0, width, height);
		}
	} catch (...) {
		HPDF_Free (pdf);
		return 1;
	}
	std::cout<<"DONE!\n";
	HPDF_SaveToFile(pdf, argv[argc-1]);
	HPDF_Free (pdf);
	return 0;
}
