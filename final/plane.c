/* include the library header files */
#include <stdlib.h>
#include <stdio.h>
#include <freeglut.h>
#include <math.h>

typedef struct{
	int vertices[20];
	int count;
} Face;

#define CAMERA_STILL 0
#define CAMERA_UP 1
#define CAMERA_DOWN 2
#define CAMERA_LEFT 3
#define CAMERA_RIGHT 4
#define CAMERA_FRONT 5
#define CAMERA_BACK 6
#define _CRT_SECURE_NO_WARNINGS

// window dimensions
GLint windowWidth = 800;
GLint windowHeight = 600;
GLint windowWidth1 = 800;
GLint windowHeight1 = 600;
GLfloat lightPosition[] = { 10.0, 10.0, 10.0, 1.0 };
// variables to draw the plane and propellers
GLfloat planeVertices[6763][3];
GLfloat planeNormals[6763][3];
Face planeFaces[33][2000];
Face propellerFaces[2][200];
GLfloat propellerVertices[6763][3];
GLfloat propellerNormal[6763][3];
// color vectors with alpha 
GLfloat yellow[] = { 1.0, 1.0, 0.0 , 1.0 };
GLfloat black[] = { 0.0, 0.0, 0.0, 0.5 };
GLfloat blue[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat emission1[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat defaultEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightPurple[] = { 0.588, 0.435, 0.9, 1.0 };
// shininess of the plane
GLfloat shininess = 20.0;
// plane moving speed
GLfloat speed;
// the moving direction of the plane
GLfloat currentMovingAngle;
// the increment ratio of angle based on mouse position
GLfloat angularSpeed;
// the plane position
GLfloat planePos[3];
// the plane tilting angle
GLfloat tiltingAngle;
// switches to be controlled by keyboard input
int cameraUpAndDownSwitch;
int textureSwitch;
int fogSwitch;
int skySwitch;
int mountainSwitch;
// polygonMode switch
int polygonMode = GL_FILL;
// full screen switch
int fullScreen = 0;
// the position of the propellers
GLfloat prop1Pos[3];
GLfloat prop2Pos[3];
// mountain models' variables
GLfloat mountainVertices[41][41][3];
GLfloat mountainNormals[41][41][3];
// self-rotation angle of the propeller
GLint propellerRotateAngle;
// texture identities
GLint skyTex, seaTex, mountainTex;
// fog color vector
GLfloat fogColor[4] = { 1.0, 0.71, 0.75, 0.5 };
// the position of balls to be collected
GLfloat balls[10][6];
// variables to save the image width and height of each texture
int imageWidth, imageHeight, imageWidth1, imageHeight1, imageWidth2, imageHeight2;
// variables to save the images
GLubyte* imageData;
GLubyte* seaData;
GLubyte* mountainData;

// create a random number within min and max
float getRandomValue(float min, float max) {
	return min + (max - min) * ((float)rand() / RAND_MAX);
}

// recursive function to generate the height of each point of the mountain
void generateMountainHeight(int x1, int x2, int y1, int y2, int level, GLfloat hMax, GLfloat addedHeight) {
	if (level == 0) {
		return;
	}

	int midPoint1 = (x1 + x2) / 2;
	int midPoint2 = (y1 + y2) / 2;
	mountainVertices[midPoint1][midPoint2][1] = getRandomValue(0.0, hMax) + 1/2 * addedHeight;
	generateMountainHeight(x1, midPoint1, y1, midPoint2, level-1, hMax/2.0, mountainVertices[midPoint1][midPoint2][1]);
	generateMountainHeight(midPoint1, x2, y1, midPoint2, level-1, hMax/2.0, mountainVertices[midPoint1][midPoint2][1]);
	generateMountainHeight(x1, midPoint1, midPoint2, y2, level -1, hMax/2.0, mountainVertices[midPoint1][midPoint2][1]);
	generateMountainHeight(midPoint1, x2, midPoint2, y2, level - 1, hMax / 2.0, mountainVertices[midPoint1][midPoint2][1]);

}

// generate the x and z coordinates of the mountain
void generateMountainXandZ(void) {
	for (int i = 0; i < 41; i++) {
		for (int j = 0; j < 41; j++) {
			mountainVertices[i][j][0] = i * 0.4;
			mountainVertices[i][j][2] = j * 0.4;
		}
	}
}

// compute the normals of each points in the mountain
void computeMountainNormals(void) {
	int i, j;

	for (i = 0; i < 40; i++) {
		for (j = 0; j < 40; j++) {
			GLfloat v1[] = {mountainVertices[i][j][0], mountainVertices[i][j][1],mountainVertices[i][j][2]};
			GLfloat v2[] = {mountainVertices[i + 1][j][0], mountainVertices[i + 1][j][1],mountainVertices[i + 1][j][2]};
			GLfloat v3[] = {mountainVertices[i][j + 1][0], mountainVertices[i][j + 1][1],mountainVertices[i][j + 1][2]};

			GLfloat normal1[] = {
				(v2[2] - v1[2])* (v3[1] - v1[1]) - (v2[1] - v1[1]) * (v3[2] - v1[2]),
				(v2[0] - v1[0])* (v3[2] - v1[2]) - (v2[2] - v1[2]) * (v3[0] - v1[0]),
				(v2[1] - v1[1])* (v3[0] - v1[0]) - (v2[0] - v1[0]) * (v3[1] - v1[1])};

			GLfloat v4[] = {mountainVertices[i + 1][j + 1][0], mountainVertices[i + 1][j + 1][1],mountainVertices[i + 1][j + 1][2]};
			GLfloat normal2[] = {
				(v3[2] - v2[2])* (v4[1] - v2[1]) - (v3[1] - v2[1]) * (v4[2] - v2[2]),
				(v3[0] - v2[0])* (v4[2] - v2[2]) - (v3[2] - v2[2]) * (v4[0] - v2[0]),
				(v3[1] - v2[1])* (v4[0] - v2[0]) - (v3[0] - v2[0]) * (v4[1] - v2[1])
			};

			mountainNormals[i][j][0] = normal1[0];
			mountainNormals[i][j][1] = normal1[1];
			mountainNormals[i][j][2] = normal1[2];
			mountainNormals[i + 1][j][0] = normal1[0];
			mountainNormals[i + 1][j][1] = normal1[1];
			mountainNormals[i + 1][j][2] = normal1[2];
			mountainNormals[i][j + 1][0] = normal1[0];
			mountainNormals[i][j + 1][1] = normal1[1];
			mountainNormals[i][j + 1][2] = normal1[2];
			mountainNormals[i + 1][j + 1][0] = normal2[0];
			mountainNormals[i + 1][j + 1][1] = normal2[1];
			mountainNormals[i + 1][j + 1][2] = normal2[2];
		}
	}
}

// called when first run the code
void initializeGL(void)
{

	// load the plane from the text file
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	FILE* file;
	fopen_s(&file, "cessna.txt", "r");
	int vertexCount = 0;
	int normalCount = 0;
	int faceCount = 0;
	int subobjectCount = -1;
	
	char type;
	while (fscanf_s(file, "%c", &type, 1) != EOF) {
		switch (type) {
		case 'v':
			// Read and process vertices
			fscanf_s(file, "%f %f %f\n", &planeVertices[vertexCount][0], &planeVertices[vertexCount][1], &planeVertices[vertexCount][2]);
			vertexCount++;
			break;
		case 'n':
			// Read and process normals
			fscanf_s(file, "%f %f %f\n", &planeNormals[normalCount][0], &planeNormals[normalCount][1], &planeNormals[normalCount][2]);
			normalCount++;
			break;
		case 'f': {
			// Read and process faces
			Face currentFace;
			
			currentFace.count = 0;
			while (fscanf_s(file, "%d", &currentFace.vertices[currentFace.count]) == 1) {
				currentFace.count++;
			}
			planeFaces[subobjectCount][faceCount] = currentFace;

			faceCount++;
			break; 
		}
		case 'g':
			subobjectCount++;
			faceCount = 0;
			break;
		
		default:
			// Skip the rest of the line for other types
			while (fgetc(file) != '\n') {
				// Do nothing, just move to the next line
			}
		}
	}
	fclose(file);
	
	FILE* file1;
	
	// load the propellers from the text file
	fopen_s(&file1, "propeller.txt", "r");
	int vertexCount1 = 0;
	int normalCount1 = 0;
	int faceCount1 = 0;
	int subobjectCount1 = -1;

	while (fscanf_s(file1, "%c", &type, 1) != EOF) {
		switch (type) {
		case 'v':
			// Read and process vertices
			fscanf_s(file1, "%f %f %f\n", &propellerVertices[vertexCount1][0], &propellerVertices[vertexCount1][1], &propellerVertices[vertexCount1][2]);
			vertexCount1++;
			break;
		case 'n':
			// Read and process normals
			fscanf_s(file1, "%f %f %f\n", &propellerNormal[normalCount1][0], &propellerNormal[normalCount1][1], &propellerNormal[normalCount1][2]);
			normalCount1++;
			break;
		case 'f': {
			// Read and process faces
			Face currentFace;
			
			currentFace.count = 0;
			while (fscanf_s(file1, "%d", &currentFace.vertices[currentFace.count]) == 1) {
				currentFace.count++;
			}
			propellerFaces[subobjectCount1][faceCount1] = currentFace;

			faceCount1++;
			break; 
		}
		case 'g':
			subobjectCount1++;
			faceCount1 = 0;
			break;
		
		default:
			// Skip the rest of the line for other types
			while (fgetc(file1) != '\n') {
				// Do nothing, just move to the next line
			}
		}
	}

	fclose(file1);

	// define the light color and intensity
	GLfloat ambientLight[] = { 0.0, 0.0, 0.0, 1.0 };  // relying on global ambient
	GLfloat diffuseLight[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specularLight[] = { 1.0, 1.0, 1.0, 1.0 };
	//  the global ambient light level
	GLfloat globalAmbientLight[] = { 0.4, 0.4, 0.4, 1.0 };

	// set the global ambient light level
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientLight);

	// define the color and intensity for light 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, diffuseLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, specularLight);

	// enable lighting 
	glEnable(GL_LIGHTING);
	// enable light 0
	glEnable(GL_LIGHT0);


	// turn on depth testing so that polygons are drawn in the correct order
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_SMOOTH);

	// make sure the normals are unit vectors
	glEnable(GL_NORMALIZE);

	// generate the position of balls
	for (int index = 0; index < 10; index++) {
		balls[index][0] = getRandomValue(-50.0, 50.0);
		balls[index][1] = getRandomValue(-50.0, 50.0);
		balls[index][2] = getRandomValue(0.0, 1.0);
		balls[index][3] = getRandomValue(0.0, 1.0);
		balls[index][4] = getRandomValue(0.0, 1.0);
	}

	// set the default values
	fogSwitch = 0;
	speed = 0.01;
	cameraUpAndDownSwitch = CAMERA_STILL;
	textureSwitch = 0;
	skySwitch = 1;
	prop1Pos[0] = planePos[0] - 0.355;
	prop1Pos[1] = planePos[1] - 0.15;
	prop1Pos[2] = planePos[2];
	prop2Pos[0] = planePos[0] + 0.355;
	prop2Pos[1] = planePos[1] - 0.15;
	prop2Pos[2] = planePos[2];

	// generate the mountain
	generateMountainHeight(0, 40, 0, 40, 6, 10.0, 20.0);
	generateMountainXandZ();
	computeMountainNormals();

	// print the instructions
	printf("Scene Controls\n");
	printf("---------------- -\n");
	printf("f: toggle fullscreen\n");
	printf("b : toggle fog\n");
	printf("m : toggle mountains\n");
	printf("t : toggle mountain texture\n");
	printf("s : toggle sea & sky\n");
	printf("w : toggle wire frame\n");
	printf("q : quit\n\n");

	printf(" Camera Controls\n");
	printf("---------------- -\n");
	printf("Page Up : faster\n");
	printf("Page Down : slower\n");
	printf("Up    Arrow : move up\n");
	printf("Down  Arrow : move down\n");
	printf("Mouse Right : move right\n");
	printf("Mouse Left : move left\n");

	printf("To win the game, collect all stars in the map! :)))))");

}

//Image loading function, cited from CSCI 3161 website by Dr. Brooks
void loadSea(void)
{
	// the ID of the image file
	FILE* fileID;

	// maxValue
	int  maxValue;

	// total number of pixels in the image
	int  totalPixels;

	// temporary character
	char tempChar;

	// counter variable for the current pixel in the image
	int i;

	// array for reading in header information
	char headerLine[100];

	// if the original values are larger than 255
	float RGBScaling;

	// temporary variables for reading in the red, green and blue data of each pixel
	int red, green, blue;

	// open the image file for reading
	fileID = fopen("sea02.ppm", "r");

	// read in the first header line
	//    - "%[^\n]"  matches a string of all characters not equal to the new line character ('\n')
	//    - so we are just reading everything up to the first line break
	fscanf(fileID, "%[^\n] ", headerLine);

	// make sure that the image begins with 'P3', which signifies a PPM file
	if ((headerLine[0] != 'P') || (headerLine[1] != '3'))
	{
		printf("This is not a PPM file!\n");
		exit(0);
	}

	// read in the first character of the next line
	fscanf(fileID, "%c", &tempChar);

	// while we still have comment lines (which begin with #)
	while (tempChar == '#')
	{
		// read in the comment
		fscanf(fileID, "%[^\n] ", headerLine);

		// read in the first character of the next line
		fscanf(fileID, "%c", &tempChar);
	}

	// the last one was not a comment character '#', so we need to put it back into the file stream (undo)
	ungetc(tempChar, fileID);

	// read in the image hieght, width and the maximum value
	fscanf(fileID, "%d %d %d", &imageWidth1, &imageHeight1, &maxValue);

	// compute the total number of pixels in the image
	totalPixels = imageWidth1 * imageHeight1;

	// allocate enough memory for the image  (3*) because of the RGB data
	seaData = malloc(3 * sizeof(GLuint) * totalPixels);


	// determine the scaling for RGB values
	RGBScaling = 255.0 / maxValue;


	// if the maxValue is 255 then we do not need to scale the 
	//    image data values to be in the range or 0 to 255
	if (maxValue == 255)
	{
		for (i = 0; i < totalPixels; i++)
		{
			// read in the current pixel from the file
			fscanf(fileID, "%d %d %d", &red, &green, &blue);

			// store the red, green and blue data of the current pixel in the data array
			seaData[3 * totalPixels - 3 * i - 3] = red;
			seaData[3 * totalPixels - 3 * i - 2] = green;
			seaData[3 * totalPixels - 3 * i - 1] = blue;
		}
	}
	else  // need to scale up the data values
	{
		for (i = 0; i < totalPixels; i++)
		{
			// read in the current pixel from the file
			fscanf(fileID, "%d %d %d", &red, &green, &blue);

			// store the red, green and blue data of the current pixel in the data array
			seaData[3 * totalPixels - 3 * i - 3] = red * RGBScaling;
			seaData[3 * totalPixels - 3 * i - 2] = green * RGBScaling;
			seaData[3 * totalPixels - 3 * i - 1] = blue * RGBScaling;
		}
	}


	// close the image file
	fclose(fileID);

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &seaTex);
	glBindTexture(GL_TEXTURE_2D, seaTex);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, imageWidth1, imageHeight1, GL_RGB, GL_UNSIGNED_BYTE, seaData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
}

//Image loading function, cited from CSCI 3161 website by Dr. Brooks
void loadSky(void)
{
	// the ID of the image file
	FILE* fileID;

	// maxValue
	int  maxValue;

	// total number of pixels in the image
	int  totalPixels;

	// temporary character
	char tempChar;

	// counter variable for the current pixel in the image
	int i;

	// array for reading in header information
	char headerLine[100];

	// if the original values are larger than 255
	float RGBScaling;

	// temporary variables for reading in the red, green and blue data of each pixel
	int red, green, blue;

	// open the image file for reading
	fileID = fopen("sky08.ppm", "r");

	// read in the first header line
	//    - "%[^\n]"  matches a string of all characters not equal to the new line character ('\n')
	//    - so we are just reading everything up to the first line break
	fscanf(fileID, "%[^\n] ", headerLine);

	// make sure that the image begins with 'P3', which signifies a PPM file
	if ((headerLine[0] != 'P') || (headerLine[1] != '3'))
	{
		printf("This is not a PPM file!\n");
		exit(0);
	}

	// read in the first character of the next line
	fscanf(fileID, "%c", &tempChar);

	// while we still have comment lines (which begin with #)
	while (tempChar == '#')
	{
		// read in the comment
		fscanf(fileID, "%[^\n] ", headerLine);
		// read in the first character of the next line
		fscanf(fileID, "%c", &tempChar);
	}

	// the last one was not a comment character '#', so we need to put it back into the file stream (undo)
	ungetc(tempChar, fileID);

	// read in the image hieght, width and the maximum value
	fscanf(fileID, "%d %d %d", &imageWidth, &imageHeight, &maxValue);

	// compute the total number of pixels in the image
	totalPixels = imageWidth * imageHeight;

	// allocate enough memory for the image  (3*) because of the RGB data
	imageData = malloc(3 * sizeof(GLuint) * totalPixels);


	// determine the scaling for RGB values
	RGBScaling = 255.0 / maxValue;


	// if the maxValue is 255 then we do not need to scale the 
	//    image data values to be in the range or 0 to 255
	if (maxValue == 255)
	{
		for (i = 0; i < totalPixels; i++)
		{
			// read in the current pixel from the file
			fscanf(fileID, "%d %d %d", &red, &green, &blue);

			// store the red, green and blue data of the current pixel in the data array
			imageData[3 * totalPixels - 3 * i - 3] = red;
			imageData[3 * totalPixels - 3 * i - 2] = green;
			imageData[3 * totalPixels - 3 * i - 1] = blue;
		}
	}
	else  // need to scale up the data values
	{
		for (i = 0; i < totalPixels; i++)
		{
			// read in the current pixel from the file
			fscanf(fileID, "%d %d %d", &red, &green, &blue);

			// store the red, green and blue data of the current pixel in the data array
			imageData[3 * totalPixels - 3 * i - 3] = red * RGBScaling;
			imageData[3 * totalPixels - 3 * i - 2] = green * RGBScaling;
			imageData[3 * totalPixels - 3 * i - 1] = blue * RGBScaling;
		}
	}


	// close the image file
	fclose(fileID);

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &skyTex);
	glBindTexture(GL_TEXTURE_2D, skyTex);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

//Image loading function, cited from CSCI 3161 website by Dr. Brooks
void loadMountain(void) {
	// the ID of the image file
	glColor3f(1.0, 1.0, 1.0);
	FILE* fileID;

	// maxValue
	int  maxValue;

	// total number of pixels in the image
	int  totalPixels;

	// temporary character
	char tempChar;

	// counter variable for the current pixel in the image
	int i;

	// array for reading in header information
	char headerLine[100];

	// if the original values are larger than 255
	float RGBScaling;

	// temporary variables for reading in the red, green and blue data of each pixel
	int red, green, blue;

	// open the image file for reading
	fileID = fopen("mount03.ppm", "r");

	// read in the first header line
	//    - "%[^\n]"  matches a string of all characters not equal to the new line character ('\n')
	//    - so we are just reading everything up to the first line break
	fscanf(fileID, "%[^\n] ", headerLine);

	// make sure that the image begins with 'P3', which signifies a PPM file
	if ((headerLine[0] != 'P') || (headerLine[1] != '3'))
	{
		exit(0);
	}

	// read in the first character of the next line
	fscanf(fileID, "%c", &tempChar);

	// while we still have comment lines (which begin with #)
	while (tempChar == '#')
	{
		// read in the comment
		fscanf(fileID, "%[^\n] ", headerLine);
		// read in the first character of the next line
		fscanf(fileID, "%c", &tempChar);
	}

	// the last one was not a comment character '#', so we need to put it back into the file stream (undo)
	ungetc(tempChar, fileID);

	// read in the image hieght, width and the maximum value
	fscanf(fileID, "%d %d %d", &imageWidth2, &imageHeight2, &maxValue);

	// compute the total number of pixels in the image
	totalPixels = imageWidth2 * imageHeight2;

	// allocate enough memory for the image  (3*) because of the RGB data
	mountainData = malloc(3 * sizeof(GLuint) * totalPixels);


	// determine the scaling for RGB values
	RGBScaling = 255.0 / maxValue;


	// if the maxValue is 255 then we do not need to scale the 
	//    image data values to be in the range or 0 to 255
	if (maxValue == 255)
	{
		for (i = 0; i < totalPixels; i++)
		{
			// read in the current pixel from the file
			fscanf(fileID, "%d %d %d", &red, &green, &blue);

			// store the red, green and blue data of the current pixel in the data array
			mountainData[3 * totalPixels - 3 * i - 3] = red;
			mountainData[3 * totalPixels - 3 * i - 2] = green;
			mountainData[3 * totalPixels - 3 * i - 1] = blue;
		}
	}
	else  // need to scale up the data values
	{
		for (i = 0; i < totalPixels; i++)
		{
			// read in the current pixel from the file
			fscanf(fileID, "%d %d %d", &red, &green, &blue);

			// store the red, green and blue data of the current pixel in the data array
			mountainData[3 * totalPixels - 3 * i - 3] = red * RGBScaling;
			mountainData[3 * totalPixels - 3 * i - 2] = green * RGBScaling;
			mountainData[3 * totalPixels - 3 * i - 1] = blue * RGBScaling;
		}
	}

	fclose(fileID);

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &mountainTex);
	glBindTexture(GL_TEXTURE_2D, mountainTex);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE, mountainData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

// draw the plane according to the file input
void drawMyPlane(void) {

	int facesInEachSubobject[33];
	int totalFaces = 0;

	for (int i = 0; i < 33; i++) {
		int j = 0;
		facesInEachSubobject[i] = 0;
		while (planeFaces[i][j].count > 0) {
			facesInEachSubobject[i]++;
			totalFaces++;
			j++;
		}
	}

	for (int i = 0; i < 33; i++) {
		if (i < 3 || (i <= 13 && i >= 8) || (i <= 32 && i >= 26)) {
			
			
			glMaterialfv(GL_FRONT, GL_AMBIENT, yellow);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
			glMaterialfv(GL_FRONT, GL_SPECULAR, yellow);
			glMaterialf(GL_FRONT, GL_SHININESS, shininess);
		}
		else if (i == 7 || (i <= 25 && i >= 14)) {
			glMaterialfv(GL_FRONT, GL_AMBIENT, blue);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
			glMaterialfv(GL_FRONT, GL_SPECULAR, blue);
			glMaterialf(GL_FRONT, GL_SHININESS, shininess);
		}
		else if (i == 4 || i == 5) {
			glMaterialfv(GL_FRONT, GL_AMBIENT, black);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, black);
			glMaterialfv(GL_FRONT, GL_SPECULAR, black);
			glMaterialf(GL_FRONT, GL_SHININESS, shininess);
		}
		else if (i == 6) {
			glMaterialfv(GL_FRONT, GL_AMBIENT, lightPurple);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, lightPurple);
			glMaterialfv(GL_FRONT, GL_SPECULAR, lightPurple);
			glMaterialf(GL_FRONT, GL_SHININESS, shininess);
		}


		for (int j = 0; j < facesInEachSubobject[i]; j++) {
			glBegin(GL_POLYGON);
			for (int k = 0; k < planeFaces[i][j].count; k++) {
				int curVertex = planeFaces[i][j].vertices[k] - 1;
				if (curVertex >= 0 && curVertex < 6763) {
					glNormal3f(planeNormals[curVertex][0], planeNormals[curVertex][1], planeNormals[curVertex][2]);
					glVertex3f(planeVertices[curVertex][0], planeVertices[curVertex][1], planeVertices[curVertex][2]);

				}
				else {
					printf(stderr, "Error: Invalid vertex index\n");
				}
			}
			glEnd();

		}
	}
}

// draw the mountains 
void drawMountains() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, blue);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialf(GL_FRONT, GL_SHININESS, 8.0);
	if (textureSwitch == 1) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mountainTex);
	}
	
	for (int i = 0; i < 40; i++) {
		for (int j = 0; j < 40; j++) {
			glBegin(GL_POLYGON);
			glNormal3f(mountainNormals[i][j][0], mountainNormals[i][j][1], mountainNormals[i][j][2]);
			glVertex3f(mountainVertices[i][j][0], mountainVertices[i][j][1], mountainVertices[i][j][2]);
			glNormal3f(mountainNormals[i+1][j][0], mountainNormals[i+1][j][1], mountainNormals[i+1][j][2]);
			glVertex3f(mountainVertices[i+1][j][0], mountainVertices[i+1][j][1], mountainVertices[i+1][j][2]);
			glNormal3f(mountainNormals[i+1][j+1][0], mountainNormals[i+1][j+1][1], mountainNormals[i+1][j+1][2]);
			glVertex3f(mountainVertices[i+1][j+1][0], mountainVertices[i+1][j+1][1], mountainVertices[i+1][j+1][2]);
			glNormal3f(mountainNormals[i][j+1][0], mountainNormals[i][j+1][1], mountainNormals[i][j+1][2]);
			glVertex3f(mountainVertices[i][j+1][0], mountainVertices[i][j+1][1], mountainVertices[i][j+1][2]);
			glEnd();
		}
	}
	

	glDisable(GL_TEXTURE_2D);
}

// draw the propellers according to the file input
void drawPropeller(void) {
	int facesInEachSubobject[2];
	int totalFaces = 0;

	for (int i = 0; i < 2; i++) {
		int j = 0;
		facesInEachSubobject[i] = 0;
		while (propellerFaces[i][j].count > 0) {
			facesInEachSubobject[i]++;
			totalFaces++;
			j++;
		}
	}


	for (int i = 0; i < 2; i++) {

		if (i == 0) {
			glMaterialfv(GL_FRONT, GL_AMBIENT, yellow);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
			glMaterialfv(GL_FRONT, GL_SPECULAR, yellow);
			glMaterialf(GL_FRONT, GL_SHININESS, shininess);
		}
		else {
			glMaterialfv(GL_FRONT, GL_AMBIENT, red);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
			glMaterialfv(GL_FRONT, GL_SPECULAR, red);
			glMaterialf(GL_FRONT, GL_SHININESS, shininess);
		}

		for (int j = 0; j < facesInEachSubobject[i]; j++) {
			glBegin(GL_POLYGON);
			for (int k = 0; k < propellerFaces[i][j].count; k++) {
				int curVertex = propellerFaces[i][j].vertices[k] - 1;
				if (curVertex >= 0 && curVertex < 6763) {
					glNormal3f(propellerNormal[curVertex][0], propellerNormal[curVertex][1], propellerNormal[curVertex][2]);
					glVertex3f(propellerVertices[curVertex][0], propellerVertices[curVertex][1], propellerVertices[curVertex][2]);
					
				}
				else {
					printf(stderr, "Error: Invalid vertex index\n");
				}
			}
			glEnd();

		}
	}
}

/************************************************************************

	Function:		myResize

	Description:	Handles a user resize of the window.

*************************************************************************/
void myResize(int newWidth, int newHeight)
{

	windowWidth1 = windowWidth;
	windowHeight1 = windowHeight;

	// update the new width
	windowWidth = newWidth;
	// update the new height
	windowHeight = newHeight;

	// update the viewport to still be all of the window
	glViewport(0, 0, windowWidth, windowHeight);

	// change into projection mode so that we can change the camera properties
	glMatrixMode(GL_PROJECTION);

	// load the identity matrix into the projection matrix
	glLoadIdentity();

	// gluPerspective(fovy, aspect, near, far)
	gluPerspective(45, (float)windowWidth / (float)windowHeight, 0.1, 300);

	// change into model-view mode so that we can change the object positions
	glMatrixMode(GL_MODELVIEW);
}

// draw the three axis
void drawAxis(void) {
	glDisable(GL_LIGHTING);
	glLineWidth(5.0);
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(1.0, 0.0, 0.0);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 1.0);
	glEnd();
	GLUquadric* quad;
	quad = gluNewQuadric();
	glColor3f(1.0, 1.0, 1.0);
	gluSphere(quad, 0.05, 100, 100);
	glLineWidth(0.5);
	glEnable(GL_LIGHTING);
}

// draw the sea
void drawMesh(void) {

	if (polygonMode == GL_FILL) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, seaTex);
		GLUquadricObj* sea = gluNewQuadric();
		gluQuadricTexture(sea, GL_TRUE);
		gluDisk(sea, 0.01, 100, 45, 30);
		glDisable(GL_TEXTURE_2D);
		gluDeleteQuadric(sea);
	}
	else {
		GLUquadricObj* sea = gluNewQuadric();
		gluDisk(sea, 0.5, 100, 45, 30);
		gluDeleteQuadric(sea);
	}

}

// draw the sky
void drawMesh1(void) {
	if (polygonMode == GL_FILL) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, skyTex);
		GLUquadricObj* sky = gluNewQuadric();
		gluQuadricTexture(sky, GL_TRUE);
		gluCylinder(sky, 99, 99, 100, 60, 60);
		glDisable(GL_TEXTURE_2D);
		gluDeleteQuadric(sky);
	}
	else {
		GLUquadricObj* sky = gluNewQuadric();
		gluCylinder(sky, 99, 99, 100, 60, 60);
		gluDeleteQuadric(sky);
	}
}

// draw fog on the sea
void drawFog() {
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_MODE, GL_EXP);
	glFogf(GL_FOG_DENSITY, 0.005);

}

// draw the balls to be collected
void drawBalls() {
	for (int i = 0; i < 10; i++) {

		if (balls[i][5] == 0) {
			GLfloat col[] = { balls[i][2], balls[i][3], balls[i][4], 1.0 };
			glMaterialfv(GL_FRONT, GL_EMISSION, col);
			glPushMatrix();
			glTranslatef(balls[i][0], 0.0, balls[i][1]);
			GLUquadric* quad = gluNewQuadric();
			gluSphere(quad, 0.3, 30, 30);
			glPopMatrix();
		}
	}
}

// display the number of balls the user has collected
void drawScoreUI() {
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, windowWidth, 0, windowHeight);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Draw UI elements
	glColor3f(1.0, 1.0, 1.0);  // Set color to white
	glRasterPos2f(windowWidth - 150, 20);  // Position for the text
	
	char text[] = "Score: ";
	for (int i = 0; text[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
	}
	char score[3];
	int ballsCollected = 0;
	for (int i = 0; i < 10; i++) {
		if (balls[i][5] == 1) {
			ballsCollected++;
		}
	}
	
	if (ballsCollected >= 10) {
		score[0] = 'W';
		score[1] = 'i';
		score[2] = 'n';
	}
	else {
		score[0] = '\0';
		score[1] = '0' + ballsCollected;
	}
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, score[0]);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, score[1]);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, score[2]);
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);

	// load the identity matrix into the projection matrix
	glLoadIdentity();

	// gluPerspective(fovy, aspect, near, far)
	gluPerspective(45, (float)windowWidth / (float)windowHeight, 0.1, 300);

	// change into model-view mode so that we can change the object positions
	glMatrixMode(GL_MODELVIEW);
	
}

// move the plane according to its direction
void updatePlane() {
	//Update plane position based on its direction and speed

	planePos[0] += speed * sin(currentMovingAngle * 3.14159265 / 180.0f);
	planePos[2] -= speed * cos(currentMovingAngle * 3.14159265 / 180.0f);
}

// function to display all the necessary elements in the animation
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	updatePlane(); // update the plane position before draw it
	// change the position of camera based on plane position
	gluLookAt(planePos[0] - 6.0 * sin(currentMovingAngle * 3.14159265 / 180), planePos[1] + 1.5, planePos[2] + 6 * cos(currentMovingAngle * 3.14159265 / 180) ,
		planePos[0], planePos[1] + 1.5, planePos[2],
		0, 1, 0);

	// draw the basic elements of the window
	drawAxis();
	drawScoreUI();
	// position the directional light
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	
	// plane and propeller transformation
	glPushMatrix();
	glTranslatef(planePos[0], planePos[1], planePos[2]);
	glRotatef(-currentMovingAngle, 0.0, 1.0, 0.0);
	glRotatef(-tiltingAngle, 0.0, 0.0, 1.0);
	
	// plane transformation
	glPushMatrix(); 
	glRotatef(270, 0.0, 1.0, 0.0);
	glTranslatef(0.0, 0.0, 0.0);
	drawMyPlane(); 
	glPopMatrix(); 

	// propeller 1 
	glPushMatrix();
	glTranslatef(prop1Pos[0], prop1Pos[1], prop1Pos[2]);
	glRotatef(-90, 0.0, 1.0, 0.0);
	glRotatef((float)propellerRotateAngle, 1.0, 0.0, 0.0);
	glTranslatef(0.0, 0.15, -0.355);
	drawPropeller();
	glPopMatrix();

	// propeller 2
	glPushMatrix();
	glTranslatef(prop2Pos[0], prop2Pos[1], prop2Pos[2]);
	glRotatef(-90, 0.0, 1.0, 0.0);
	glRotatef((float)propellerRotateAngle, 1.0, 0.0, 0.0);
	glTranslatef(0.0, 0.15, -0.355);
	drawPropeller();
	glPopMatrix();
	glPopMatrix();

	// change the material of the sky and sea
	glMaterialfv(GL_FRONT, GL_AMBIENT, defaultEmission);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, defaultEmission);
	glMaterialfv(GL_FRONT, GL_SPECULAR, defaultEmission);
	glMaterialfv(GL_FRONT, GL_EMISSION, white);
	glMaterialf(GL_FRONT, GL_SHININESS, 0.0);

	// draw the sky and sea when the switch is turned on
	if (skySwitch == 1) {

		if (polygonMode == GL_LINE) {
			glMaterialfv(GL_FRONT, GL_EMISSION, emission1);
		}
		else {
			glMaterialfv(GL_FRONT, GL_EMISSION, white);
		}

		if (fogSwitch == 1) {
			glEnable(GL_FOG);
			drawFog();
		}
	
		glPushMatrix();
		glTranslatef(0.0, -2.0, 0.0);
		glRotatef(90, 1.0, 0.0, 0.0);
		drawMesh();
		glPopMatrix();
		glMaterialfv(GL_FRONT, GL_EMISSION, defaultEmission);

		glDisable(GL_FOG);

		GLfloat emission[] = { 0.9, 0.55, 0.0, 1.0 };
		if (polygonMode == GL_LINE) {
			glMaterialfv(GL_FRONT, GL_EMISSION, emission);
		}
		else {
			glMaterialfv(GL_FRONT, GL_EMISSION, white);
		}

		glPushMatrix();
		glTranslatef(0.0, -2.2, 0.0);
		glRotatef(270, 1.0, 0.0, 0.0);
		drawMesh1();
		glPopMatrix();
		glMaterialfv(GL_FRONT, GL_EMISSION, defaultEmission);
	}

	// draw the balls
	drawBalls();

	// draw the three mountains when toggle on  
	glMaterialfv(GL_FRONT, GL_EMISSION, defaultEmission);
	if (mountainSwitch == 1) {
		glPushMatrix();
		glTranslatef(20.0, -1.0, 10.0);
		drawMountains();
		glPopMatrix();
		drawMountains();
		glPushMatrix();
		glTranslatef(-20.0, -1.0, -8.0);
		drawMountains();
		glPopMatrix();
	}

	// draw the score UI
	drawScoreUI();
	// swap the double buffers
	glutSwapBuffers();
}

// callback function when no actions detected
void myIdle(void) {
	
	propellerRotateAngle = (propellerRotateAngle + 1) % 360; // propeller self-rotation
	currentMovingAngle += angularSpeed;                      // increment the direction
	// keep angle within 0 - 360
	if (currentMovingAngle > 360) {
		currentMovingAngle -= 360;
	}
	else if (currentMovingAngle < 0) {
		currentMovingAngle += 360;
	}

	// move the plane up and down if the up and down arrow keys are pressed 
	switch (cameraUpAndDownSwitch) {
	case CAMERA_STILL:
		break;

	case CAMERA_UP:
		planePos[1] += 0.001;
		break;

	case CAMERA_DOWN:
		planePos[1] -= 0.001;
		break;
	}

	// detect if the plane pick the balls up
	for (int i = 0; i < 10; i++) {
		int dx = planePos[0] - balls[i][0];
		int dy = planePos[1] - 0.0;
		int dz = planePos[2] - balls[i][1];
		if (sqrt(dx * dx + dy * dy + dz * dz) < 0.3) {
			balls[i][5] = 1;
		}
	}

	// refresh the window
	glutPostRedisplay();
}

/************************************************************************

	Function:		myKeys

	Description:	Handles keyboard input.

*************************************************************************/
void myKeys(unsigned char key, int x, int y)
{

	if (key == 'w' || key == 'W') {
		if (polygonMode == GL_FILL) {
			polygonMode = GL_LINE; 
		}
		else {
			polygonMode = GL_FILL; 
		}
		glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	}
	
	if (key == 'f' || key == 'F') {
		if (fullScreen == 1) {
			fullScreen = 0;
			glutFullScreen();
		}
		else {
			fullScreen = 1;
			glutReshapeWindow(windowWidth1, windowHeight1);
		}
	}

	if (key == 't' || key == 'T') {
		if (textureSwitch == 0) {
			textureSwitch = 1;
		}
		else {
			textureSwitch = 0;
		}
	}

	if (key == 'b' || key == 'B') {
		if (fogSwitch == 0) {
			fogSwitch = 1;
		}
		else {
			fogSwitch = 0;
		}
	}

	if (key == 's' || key == 'S') {
		if (skySwitch == 0) {
			skySwitch = 1;
		}
		else {
			skySwitch = 0;
		}
	}

	if (key == 'M' || key == 'm') {
		if (mountainSwitch == 0) {
			mountainSwitch = 1;
		}
		else {
			mountainSwitch = 0;
		}
	}

	if ((key == 'q') || (key == 'Q'))
		exit(0);

	glutPostRedisplay();
}

// handles special keys like arrows and page up 
void mySpecialKey(unsigned char key, int x, int y) {
	
	if (key == GLUT_KEY_PAGE_UP) {
		if (speed < 0.02) {
			speed += 0.002;
			if (speed > 0.02)
				speed = 0.02;
		}
		else {
			speed = 0.02;
		}
	}
	
	if (key == GLUT_KEY_PAGE_DOWN) {
		if (speed > 0.002) {
			speed -= 0.002;
			if (speed < 0.002)
				speed = 0.002;
		}
		else {
			speed = 0.002;
		}
	}

	if (key == GLUT_KEY_UP) {
		cameraUpAndDownSwitch = CAMERA_UP;
	}

	if (key == GLUT_KEY_DOWN) {
		cameraUpAndDownSwitch = CAMERA_DOWN;
	}

	glutPostRedisplay();
}

// call backfunction to control countinously pressed key
void myKeyUp(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN:
		cameraUpAndDownSwitch = CAMERA_STILL;
		break;
	}
}

// controls the movement and tilting angle of the plane 
void myMouse(int x, int y) {
	
	int centerX = windowWidth/ 2;

	float deltaX = ((float)x - (float)centerX) / centerX;

	// Update plane orientation based on mouse motion
	if (deltaX < 0.1f && deltaX > -0.1f) {
		angularSpeed = 0.0;
		tiltingAngle = 0.0;
	}
	else {
		angularSpeed = (deltaX - 0.1) * 0.5f;
		tiltingAngle = (deltaX - 0.1) * 45.0;
	}

	glutPostRedisplay();
}

/************************************************************************

	Function:		main

	Description:	Sets up the openGL rendering context and the windowing
					system, then begins the display loop.

*************************************************************************/
void main(int argc, char** argv)
{
	// initialize the toolkit
	glutInit(&argc, argv);
	// set display mode
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	// set window size
	glutInitWindowSize(windowWidth, windowHeight);
	// set window position on screen
	glutInitWindowPosition(100, 100);
	// open the screen window
	glutCreateWindow("Airplane");
	loadSea();
	loadSky();
	loadMountain();
	// register redraw function
	glutDisplayFunc(myDisplay);
	// register keyboard handler
	glutKeyboardFunc(myKeys);
	glutSpecialFunc(mySpecialKey);
	glutSpecialUpFunc(myKeyUp); // call when arrow and page key loosed
	// register the resize function
	glutReshapeFunc(myResize);
	glutPassiveMotionFunc(myMouse);
	glutIdleFunc(myIdle);
	//initialize the rendering context
	initializeGL();
	// go into a perpetual loop
	glutMainLoop();
}
