//
// Created by qidian on 3/2/18.
//

#include "Object.h"
/******************************************************************************
	DEFAULT CONSTRUCTOR
	purpose: creates default instance of Object
	--------------------------------------------------------------------------
	requires: nothing
	ensures: instance of Object created such that numVert = numNorm = numFaces
			 = numIndex = 0;
			 title set to default value
******************************************************************************/
Object::Object()
{
    numVert = 0;
    numNorm = 0;
    numFaces = 0;
    numIndex = 0;
    title = "default";
}

/******************************************************************************
	PARAMETERIZED CONSTRUCTOR
	purpose: create instance of Object with title set to specified fileName
	--------------------------------------------------------------------------
	requires: fileName
	ensures: instance of Object created such that numVert = numNorm = numFaces
			 = numIndex = 0;
			 title = #fileName
******************************************************************************/
Object::Object(std::string fileName)
{
    numVert = 0;
    numNorm = 0;
    numFaces = 0;
    numIndex = 0;
    title = fileName;
}

/******************************************************************************
	LOAD
	purpose: to load the Object from the specified .obj file onto the GPU
	--------------------------------------------------------------------------
	requires: program
	ensures: Object information is loaded onto the GPU
******************************************************************************/
void Object::load(GLuint & program)
{
    getObjInfo(title);
    // now we need to create a buffer object
    unsigned long size = (numVert * sizeof(glm::vec4));

    // make sure to put the color and the vertices into the same buffer
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    glBufferData(GL_ARRAY_BUFFER, size + size, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, vert);
    glBufferSubData(GL_ARRAY_BUFFER, size, size, color);

    // load the index buffer for the vertices
    glGenBuffers(1, &IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
    // Because GL_ELEMENT_ARRAY_BUFFER deals in vertex array indices we need to multiply by
    // the number of elements in each vertex (we are using an array of vec4s) to load ALL
    // of our data instead of only the first quarter
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndex * 4, index, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    // complete all the plumbing stuffs
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);

    /*glVertexAttribPointer(
        GLuint attribute_v_color,	(attribute)
        GLint 3,					(number of elements per vertex, here (r,g,b))
        GLenum GL_FLOAT,			(the type of each element)
        GLboolean GL_FALSE,			(take our values as-is)
        GLsizei 0,                  (no extra data between each position)
        const GLvoid* 0             (offset of first element)
    );*/
    glVertexAttribPointer(vPosition, 4, GL_FLOAT,
                          GL_FALSE, 0, 0);

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
                          (GLvoid*)(numVert * sizeof(glm::vec4)));
} // end Object::load

/******************************************************************************
	DRAW
	purpose: to have the GPU draw the object into the buffer
	--------------------------------------------------------------------------
	requires: successful load
	ensures: Object has been drawn into buffer
******************************************************************************/
void Object::draw()
{
    glDrawArrays(GL_TRIANGLES, 0, numVert);
}

/******************************************************************************
	GETOBJINFO
	purpose: to acquire vertex, normals, and faces from the specified .obj
			 file
	--------------------------------------------------------------------------
	requires: fileName
	ensures: verticies, normals, and faces read from specified file and stored
			 in respective variables.  Basic object information displayed to
			 screen for debugging purposes.
******************************************************************************/
void Object::getObjInfo(std::string fileName)
{
    std::ifstream infile;
    std::string read; // for reading in commands
    std::string fCorner[3]; // for reading in 3 vertices / face
    std::string thisCorner;
    int digit, p;
    GLfloat x, y, z; // for reading in vertices if command == "v"

    infile.open(fileName);

    if (infile) {
        infile >> read;
        while (!infile.eof()) {
            // check to see if read is a vertex value
            if (read == "v") {
                infile >> x >> y >> z;
                tempVertList.push_back(glm::vec4(x, y, z, 1));
            } // end vertex
                // check to see if read is a vertex normal value
            else if (read == "vn") {
                infile >> x >> y >> z;
                normList.push_back(glm::vec4(x, y, z, 1));
            } // end vertex normal
                // check to see if read is a face
            else if (read == "f") {
                // Horner's method can be used to convert between different
                // positional numeral systems, in which case 'x' is the base of the
                // numeral system, and the a[i] are the digits of the base-x
                // representation of a given number.
                // So that's pretty cool, let's use it to parse these faces.
                // our number system is decimal, so x = 10
                // therefore, using Horner's rule we would simply add the
                // the incoming digits until we meet a character that is not a
                // digit = digit * 10 + thiscorner[p++] - 48
                infile >> fCorner[0] >> fCorner[1] >> fCorner[2];
                for (int i = 0; i < 3; i++) {
                    // process a corner
                    thisCorner = fCorner[i];

                    // here is where the Horner's rule magic begins
                    digit = 0;
                    p = 0;

                    // first get the vertex index number...don't forget to
                    // subtract by 1 to account for .obj numbering system
                    while (isdigit(thisCorner[p])) digit = digit * 10
                                                           + thisCorner[p++] - 48;
                    // digit now hold the vertex index...so let's stick in
                    // the vertIndex array
                    vertexIndex.push_back((digit - 1));

                    // the middle index value should be the material...but it
                    // is not in our file yet...so just update stuff
                    digit = 0;	p++;
                    while (isdigit(thisCorner[p])) digit = digit * 10 + thisCorner[p++] - 48;

                    // the last value should be the normal index
                    digit = 0;	p++;
                    while (isdigit(thisCorner[p])) digit = digit * 10 + thisCorner[p++] - 48;
                    normalsIndex.push_back((digit - 1));
                }
            } // end faces
            infile >> read;
        } // end while(!infile.eof())
        // store read data in arrays
        for (unsigned int i = 0; vertexIndex.size() > i; i++) {
            vertList.push_back(tempVertList[vertexIndex[i]]);
        }

        vert = vertList.data();
        color = vertList.data();
        normal = normList.data();
        index = vertexIndex.data();

        // get the number of vertices, normals, and indexes
        numVert = vertList.size();
        numNorm = normList.size();
        numIndex = vertList.size();

        // print out the above values for error checking purposes
        std::cout << "Vertices: " << numVert << std::endl;
        std::cout << "Normals: " << numNorm << std::endl;
        std::cout << "Indices: " << numIndex << std::endl;
    } // end if(infile)
    else {
        std::cout << "Error loading infile.  Verify file name and try again.\n";
    }
} // end Object::getObjInfo

/******************************************************************************
	DECONSTRUCTOR
	purpose: to deallocate memory
	--------------------------------------------------------------------------
	requires: instanced Object
	ensures: all memory allocated to Object returned to OS
******************************************************************************/
Object::~Object()
{
}
