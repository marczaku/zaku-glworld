// Picture.cpp

#include "stdafx.h"
#include "Picture.h"

void Picture::Load( char *filename )
{
	FILE *bmp_file;

	if( (bmp_file = fopen( filename, "rb" )) == NULL )
	{
		//exit_nofile( "picture::load( char * )", filename );
	}

	{
		char signature[ 2 ];
		fseek( bmp_file, 0, SEEK_SET );
		fread( (char *) &signature, 2, 1, bmp_file );
		if( signature[ 0 ] != 'B' || signature[ 1 ] != 'M' )
		{
			char string[ 100 ];
			sprintf_s( string, "picture::load( char * ): '%s' ist keine Datei vom Typ .BMP.\n", filename );
			//exit_error( string );
		}

		long compression;
		fseek( bmp_file, 30, SEEK_SET );
		fread( (char *) &compression, 4, 1, bmp_file );
		if( compression != 0 )
		{
			//exit_error( "picture::load( char * ): Komprimierte Dateien vom Typ .BMP werden nicht unterstützt.\n" );
		}
	}

	ushort size;
	fseek( bmp_file, 28, SEEK_SET );
	fread( (char *) &size, sizeof( size ), 1, bmp_file );

	if( size == 8 ) LoadBmp8( bmp_file );
	else if( size == 24 ) LoadBmp24( bmp_file );
	else 
	{
		char string[ 100 ];
		sprintf_s( string, "picture::load( char * ): Format der Datei '%s' wird nicht unterstützt.\n", filename );
		//exit_error( string );
	}

	fclose( bmp_file );  
}

void Picture::LoadBmp24( FILE *bmp_file )
{
	fseek( bmp_file, 18, SEEK_SET );
	fread( (char *) &xsize, sizeof( xsize ), 1, bmp_file );
	fread( (char *) &ysize, sizeof( ysize ), 1, bmp_file );

	if( (content = new Color[ xsize * ysize ]) == NULL )
	{
		//exit_nomemory( "picture::load_bmp_24()", "content[]" );
	}

	long padding = xsize % 4;

	long content_offset;
	fseek( bmp_file, 10, SEEK_SET );
	fread( (char *) &content_offset, 4, 1, bmp_file );

	fseek( bmp_file, content_offset, SEEK_SET );

	for( long y=0 ; y<ysize ; y++ )
	{
		for( long x=0 ; x<xsize ; x++ )
		{
			long offset = y * xsize + x;	

			fread( &content[ offset ].b,  1, 1, bmp_file );
			fread( &content[ offset ].g, 1, 1, bmp_file );
			fread( &content[ offset ].r,   1, 1, bmp_file );
		}

		fseek( bmp_file, padding, SEEK_CUR );
	}
}

void Picture::LoadBmp8( FILE *bmp_file )
{
	struct
	{
		uchar blue, green, red, alpha;
	} palette[ 256 ];

	fseek( bmp_file, 54, SEEK_SET );
	fread( (char *) palette, sizeof( palette ), 1, bmp_file );

	fseek( bmp_file, 18, SEEK_SET );
	fread( (char *) &xsize, sizeof( xsize ), 1, bmp_file );
	fread( (char *) &ysize, sizeof( ysize ), 1, bmp_file );

	if( (content = new Color[ xsize * ysize ]) == NULL )
	{
		//exit_nomemory( "picture::load_bmp_8()", "content[]" );
	}

	long padding;
	for( padding=0 ; ; padding++ ) 
		if( (xsize + padding) % 4 == 0 ) break;

	long content_offset;
	fseek( bmp_file, 10, SEEK_SET );
	fread( (char *) &content_offset, 4, 1, bmp_file );

	fseek( bmp_file, content_offset, SEEK_SET );
	for( long y=0 ; y<ysize ; y++ ) 
	{
		for( long x=0 ; x<xsize ; x++ ) 
		{
			uchar color_offset;
			fread( &color_offset, sizeof( color_offset ), 1, bmp_file );

			content[ y * xsize + x ].r = palette[ color_offset ].red;
			content[ y * xsize + x ].g = palette[ color_offset ].green;
			content[ y * xsize + x ].b = palette[ color_offset ].blue;
		}

		fseek( bmp_file, padding, SEEK_CUR );
	}  
}