#include <stdio.h>
#include <gd.h>
#include <string.h>
#include <omp.h>

int main() {
    //compile with gcc p4_negative_image.c -fopenmp -lgd
    int pixel, x, y, width, height;
    int red, green, blue;
    char input_file[256];  
    FILE *fp;
    gdImagePtr img;

    printf("Enter input file name : ");
    scanf("%s",input_file);

    for(int num_threads = 1; num_threads <= 8 ; num_threads = num_threads*2)
    {
        fp = fopen(input_file, "r");
        img = gdImageCreateFromPng(fp);
        
        width = gdImageSX(img);
        height = gdImageSY(img);

        omp_set_num_threads(num_threads);
        double t1 = omp_get_wtime();
        #pragma omp parallel for private(y, pixel, red, green, blue)
        for(x=0; x<width; x++) 
            for(y=0; y<height; y++) 
            {
                pixel = gdImageGetPixel(img, x, y);
                red   = 255 - gdImageRed(img, pixel);
                green = 255 - gdImageGreen(img, pixel);
                blue  = 255 - gdImageBlue(img, pixel);
                pixel = gdImageColorAllocate(img, red, green, blue);
               
                #pragma omp critical //comment out for data scope
                gdImageSetPixel(img, x, y, pixel);    
            }

        double t2 = omp_get_wtime();
        printf("%d Thread Time : %lf ms\n",num_threads, (t2 - t1) * 1000);
        
        fp=fopen("Neg.png", "w");
        gdImagePng(img, fp);
    }

    printf("File Size: %dx%d\n", width, height);
    gdImageDestroy(img);
    fclose(fp);

    return 0;
}
