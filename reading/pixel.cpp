/*******************
   Pixel.cpp: resamples an image with random pixels and draws them on screen
   Copyright 2017 Charles A. Wuethrich
 */

/* Initialize the general stuff. */
#include <allegro.h>  // You must include the Allegro Header file
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// define default initial numbers for image y, x and number
// of random samples
#define SIZEX 1920     
#define SIZEY 1084    
#define SAMPLENUMBER 900

// function that prints what should be the correct command line input
// unfortunately unfinished
void usage(void)
{
    printf("Usage:\n");
    printf(" -f<name>\n");
    printf(" -d<name>\n");
    exit (8);
}

int main(int argc, char *argv[]) 
{
// create our variables
// Note: a sprite is an small image that can be superimposed 
// and copied onto another image. A pel is a picture element
  int pelspritesize_x=11; // x_size of the sprite: default is 11
  int pelspritesize_y=11; // y_size of the sprite 
  int peloffset_x,peloffset_y; // reserved for the offset (used when creating  sprite
  int pel_color, transp_color, line_color, imagecolor;  // unused
  int pel_r,pel_g,pel_b;  // rgb values for the pel = pixel to draw
  int samples=SAMPLENUMBER;  // number of random samples
  int ii,jj,kk;
  int i,j;
  int irandx,irandy;         // integer x and y of random point 
  float step_x, step_y, cosvalue; //steps for the cos pel, cosinus value
  int icosvalue;                  // integer cosinus value for the transp bitmap
  float eval_pt_x, eval_pt_y, eval_modulo;  // floats for random x and y 
  int picsize_x,picsize_y;  // size of the picture 
  bool infileCMD = false;   // variables to check if user inputted file names
  bool outfileCMD = false;

  // command line parameters processing
  FILE *file=NULL;  // initialize a file type 
  char imgname[96];  // the image name
  char outname[96];  // the output image name

  // parse command line
        printf("Program name: %s\n", argv[0]);

        while ((argc > 1) && (argv[1][0] == '-'))
        {
            switch (argv[1][1])
            {
                case 'f':
                    printf("%s\n",&argv[1][2]);
                    break;  // f is unused

                case 'x':
                    printf("%s\n",&argv[1][2]);
                    printf("%s\n",&argv[1][2]);
		    pelspritesize_x=atoi(&argv[1][2]);
		    printf("pelsx=%d\n",pelspritesize_x);
                    break;  // x reads the size of the pel (size of sprite)

               case 'y':
 		    pelspritesize_y=atoi(&argv[1][2]);
		    printf("pelsy=%d\n",pelspritesize_y);
                    break;  // y reads the size of the pel (size of sprite)
		    
               case 's':
 		    samples=atoi(&argv[1][2]);
		    printf("samples=%d\n",samples);
                    break;  // s reads the number of random samples to take in the 
                            // original image

	       case 'i':    //move to next argument and read file name  (input file)
		    ++argv;
		    --argc;
		    strcpy(imgname,argv[1]); // copy argv to imgname
		    printf("input file name: %s\n",imgname);
		    infileCMD = true;
		    break; // this works!!

 	       case 'o':    //move to next argument and read file name   (output file)
		    ++argv;
		    --argc;
		    strcpy(outname,argv[1]); // copy argv to imgname
		    printf("output file name: %s\n",outname);
		    outfileCMD = true;
		    break; // this works!!
		    
                default:
                    printf("Wrong Argument: %s\n", argv[1]);
                    usage();
            }

            ++argv;
            --argc;
        }
  
  	allegro_init(); // Initialize Allegro 
	install_keyboard(); // Initialize keyboard routines

	
	/* 
	 * prepare large screen of maximum size... inelegant, but 
	 * if I do not define the drawing screen BEFORE loading
	 * the original image it only loads the original picture with 16 bit
	*/
	set_color_depth(32); // Set the color depth 
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, SIZEX, SIZEY, 0, 0); // Change our graphics mode to 640x480

	// set superpixel (=pel) offsets so it draws centered at the position
	peloffset_x=pelspritesize_x/2;
	peloffset_y=pelspritesize_y/2;

	// Create a bitmap, and load the picture into memory
	// check if picture file can be opened, exit otherwise
	// NOTE! ONLY WORKS WITH WIN3 BITMAPS!!!!!!! SEE README!
	BITMAP *oripic = NULL; // declare original picture and create bitmap
	if(infileCMD){
	  oripic = load_bitmap(imgname, NULL); // NULL is palette
	}
	else{
	  oripic = load_bitmap("win3cocc.bmp", NULL); // NULL is palette
	}
	if(!oripic){
	  printf("Could not load input image\n");
	  destroy_bitmap(oripic);
	  exit(1);
	}

	// get x and y size of input picture
	picsize_x = oripic->w;
	picsize_y = oripic->h;
	printf("size: %d x %d\n",picsize_x,picsize_y);

 	//set_color_depth(32); // Set the color depth 
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, picsize_x, picsize_y, 0, 0); // Change our graphics mode to the size of the input picture
	
	
	/* reserve a new bitmap for the pel_sprite to be drawn at random positions*/
       /* DeclareBITMAP called pel_sprite   */ 
	BITMAP *pel_sprite = create_bitmap(pelspritesize_x,pelspritesize_y); 
      

	
	/* create same size 8 bit bitmap for transparency values */
	BITMAP *pel_sprite_transp = create_bitmap_ex(8,pelspritesize_x,pelspritesize_y); //Declare 8 bit BITMAP
	clear(pel_sprite_transp);  // clear it
	

	// now fill properly the transparency map which is the same for all pels
	step_x=1.0/(float)(pelspritesize_x-1); // step is 1-number of pixels 
	step_y=1.0/(float)(pelspritesize_y-1);
	eval_pt_x=-1.0/2.0;  // evaluation point x and y to 1/2
	eval_pt_y=-1.0/2.0;
	cosvalue=0.0;   // initialize cosinus to zero
       /* fill transparency sprite with cos^4 valies of */
	for(i=0; i<pelspritesize_x; i++){
	  for(j=0; j<pelspritesize_y; j++){
	    eval_modulo=sqrt((eval_pt_x * eval_pt_x) + (eval_pt_y*eval_pt_y) );//sqrt(x^2+y^2)
	    if(eval_modulo<=0.50){ // compute cos only when inside circle of radius 1/2
	      eval_modulo=M_PI*eval_modulo;//was (M_PI*eval_modulo)/2 but this ain't correct
	                                 // because the interval is [-1/2,1/2]
	      cosvalue=cos(eval_modulo)*cos(eval_modulo)*cos(eval_modulo)*cos(eval_modulo);
	    // cosvalue is a float number in [0,1] 
	    }
	    else{ // set to 0 otherwise
	      cosvalue=0;
	    }
	    icosvalue=(int)(cosvalue*255); //scale for integer [0,255]
	    putpixel(pel_sprite_transp,i,j,icosvalue); // write in transparent sprite
	   
	    eval_pt_y=eval_pt_y+step_y; // increment y
	  }  // for j (cicle on whe whole transparency sprite columns)
	  eval_pt_y=-1.0/2.0; // reset y to lowest value
	  eval_pt_x=eval_pt_x+step_x;  // increment x
	  // Runs correctly: no need to print: printf("\n");
	} // for i (cicle on the whole transparency sprite rows)
/* Now we have a transparency sprite filled with cos^4 transparency values */
	

	/* initialize random seed */
	srand(time(NULL));
	// printf("Sample Number=%d\n",SAMPLENUMBER);	    
	
	for(kk=0;kk<samples;kk++){
	  pel_r=128; //later these will be assigned by the picure colors
	  pel_g=128;
	  pel_b=0;

	  irandx= rand() % SIZEX;
	  irandy= rand() % SIZEY;
	  ii=irandx;
	  jj=irandy;

	  //	every time I touch oripic it crashes DEBUG  
	  // printf("Here too 2");	
         /* Get color at random image position */      
	  imagecolor=getpixel(oripic,ii,jj);
	  pel_r= getr(imagecolor);
	  pel_g= getg(imagecolor);
	  pel_b= getb(imagecolor);
	  // printf("Here too 3");
	  //end of critical part
	  
	  // to do a sprite in another color, you need to set drawing  to solid,
	  // and turn on transparency later 
	  drawing_mode(DRAW_MODE_SOLID, NULL, 0,0);
	  /* fill with another color */
	  pel_color=makecol(pel_r,pel_g,pel_b);
	  rectfill(pel_sprite, 0,0,pelspritesize_x,pelspritesize_y,pel_color); // draw rectangle to bitmap

	  /* Combine the transparency and color sprites */
         drawing_mode(DRAW_MODE_TRANS, NULL, 0,0);
	  set_write_alpha_blender();
	  draw_trans_sprite(pel_sprite,  , 0,0); // draw from 0,0
	  set_alpha_blender();
	
	 
	  // maybe blit is better? I cannot have transparency with blit
	  draw_trans_sprite(screen, pel_sprite, irandx-peloffset_x,irandy-peloffset_y); // Draw the transp pel sprite to the buffer
	} // for kk

	
	readkey(); //Wait for a keypress, then quit

	//write output file if name has been input in command line
	if(outfileCMD){
	  save_bitmap(outname,screen,NULL);
	}

	
	/* Destroy the bitmap, and exit the program. */
	// destroy_bitmap(my_pic); //Release the bitmap data
	destroy_bitmap(pel_sprite); //Release the bitmap data
	destroy_bitmap(pel_sprite_transp); //Release the bitmap data
	destroy_bitmap(oripic); // release the picture data
	return 0; // Exit with no errors 
} 
END_OF_MAIN() // This must be called right after the closing bracket of your MAIN function. 
			  // It is Allegro specific.

