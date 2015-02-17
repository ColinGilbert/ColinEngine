#ifdef __cplusplus
extern "C" {
#endif


	/// \brief Load a shader, check for compile errors, print error messages to output log
	/// \param type Type of shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
	/// \param shaderSrc Shader source string
	/// \return A new shader object on success, 0 on failure
	//
	GLuint ESUTIL_API esLoadShader ( GLenum type, const char *shaderSrc );
	//
	///
	/// \brief Load a vertex and fragment shader, create a program object, link program.
	/// Errors output to log.
	/// \param vertShaderSrc Vertex shader source code
	/// \param fragShaderSrc Fragment shader source code
	/// \return A new program object linked with the vertex/fragment shader pair, 0 on failure
	//
	GLuint ESUTIL_API esLoadProgram ( const char *vertShaderSrc, const char *fragShaderSrc );
	//
	/// \brief Generates geometry for a sphere. Allocates memory for the vertex data and stores
	/// the results in the arrays. Generate index list for a TRIANGLE_STRIP
	/// \param numSlices The number of slices in the sphere
	/// \param vertices If not NULL, will contain array of float3 positions
	/// \param normals If not NULL, will contain array of float3 normals
	/// \param texCoords If not NULL, will contain array of float2 texCoords
	/// \param indices If not NULL, will contain the array of indices for the triangle strip
	/// \return The number of indices required for rendering the buffers (the number of indices stored in the indices array
	/// if it is not NULL ) as a GL_TRIANGLE_STRIP
	//
	int ESUTIL_API esGenSphere ( int numSlices, float radius, GLfloat **vertices, GLfloat **normals,
			GLfloat **texCoords, GLuint **indices );
	//
	/// \brief Generates geometry for a cube. Allocates memory for the vertex data and stores
	/// the results in the arrays. Generate index list for a TRIANGLES
	/// \param scale The size of the cube, use 1.0 for a unit cube.
	/// \param vertices If not NULL, will contain array of float3 positions
	/// \param normals If not NULL, will contain array of float3 normals
	/// \param texCoords If not NULL, will contain array of float2 texCoords
	/// \param indices If not NULL, will contain the array of indices for the triangle strip
	/// \return The number of indices required for rendering the buffers (the number of indices stored in the indices array
	/// if it is not NULL ) as a GL_TRIANGLES
	//
	int ESUTIL_API esGenCube ( float scale, GLfloat **vertices, GLfloat **normals,
			GLfloat **texCoords, GLuint **indices );
	//
	/// \brief Generates a square grid consisting of triangles. Allocates memory for the vertex data and stores
	/// the results in the arrays. Generate index list as TRIANGLES.
	/// \param size create a grid of size by size (number of triangles = (size-1)*(size-1)*2)
	/// \param vertices If not NULL, will contain array of float3 positions
	/// \param indices If not NULL, will contain the array of indices for the triangle strip
	/// \return The number of indices required for rendering the buffers (the number of indices stored in the indices array
	/// if it is not NULL ) as a GL_TRIANGLES
	//
	int ESUTIL_API esGenSquareGrid ( int size, GLfloat **vertices, GLuint **indices );
	//
	/// \brief Loads a 8-bit, 24-bit or 32-bit TGA image from a file
	/// \param ioContext Context related to IO facility on the platform
	/// \param fileName Name of the file on disk
	/// \param width Width of loaded image in pixels
	/// \param height Height of loaded image in pixels
	/// \return Pointer to loaded image. NULL on failure.
	//
	char *ESUTIL_API esLoadTGA ( void *ioContext, const char *fileName, int *width, int *height );
	//
	/// \brief multiply matrix specified by result with a scaling matrix and return new matrix in result
	/// \param result Specifies the input matrix. Scaled matrix is returned in result.
	/// \param sx, sy, sz Scale factors along the x, y and z axes respectively
	//
	void ESUTIL_API esScale ( ESMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz );
	//
	/// \brief multiply matrix specified by result with a translation matrix and return new matrix in result
	/// \param result Specifies the input matrix. Translated matrix is returned in result.
	/// \param tx, ty, tz Scale factors along the x, y and z axes respectively
	//
	void ESUTIL_API esTranslate ( ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz );
	//
	/// \brief multiply matrix specified by result with a rotation matrix and return new matrix in result
	/// \param result Specifies the input matrix. Rotated matrix is returned in result.
	/// \param angle Specifies the angle of rotation, in degrees.
	/// \param x, y, z Specify the x, y and z coordinates of a vector, respectively
	//
	void ESUTIL_API esRotate ( ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z );
	//
	// \brief multiply matrix specified by result with a perspective matrix and return new matrix in result
	/// \param result Specifies the input matrix. new matrix is returned in result.
	/// \param left, right Coordinates for the left and right vertical clipping planes
	/// \param bottom, top Coordinates for the bottom and top horizontal clipping planes
	/// \param nearZ, farZ Distances to the near and far depth clipping planes. Both distances must be positive.
	//
	void ESUTIL_API esFrustum ( ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ );
	//
	/// \brief multiply matrix specified by result with a perspective matrix and return new matrix in result
	/// \param result Specifies the input matrix. new matrix is returned in result.
	/// \param fovy Field of view y angle in degrees
	/// \param aspect Aspect ratio of screen
	/// \param nearZ Near plane distance
	/// \param farZ Far plane distance
	//
	void ESUTIL_API esPerspective ( ESMatrix *result, float fovy, float aspect, float nearZ, float farZ );
	//
	/// \brief multiply matrix specified by result with a perspective matrix and return new matrix in result
	/// \param result Specifies the input matrix. new matrix is returned in result.
	/// \param left, right Coordinates for the left and right vertical clipping planes
	/// \param bottom, top Coordinates for the bottom and top horizontal clipping planes
	/// \param nearZ, farZ Distances to the near and far depth clipping planes. These values are negative if plane is behind the viewer
	//
	void ESUTIL_API esOrtho ( ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ );
	//
	/// \brief perform the following operation - result matrix = srcA matrix * srcB matrix
	/// \param result Returns multiplied matrix
	/// \param srcA, srcB Input matrices to be multiplied
	//
	void ESUTIL_API esMatrixMultiply ( ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB );
	//
	//// \brief return an indentity matrix
	//// \param result returns identity matrix
	//
	void ESUTIL_API esMatrixLoadIdentity ( ESMatrix *result );
	//
	/// \brief Generate a transformation matrix from eye position, look at and up vectors
	/// \param result Returns transformation matrix
	/// \param posX, posY, posZ eye position
	/// \param lookAtX, lookAtY, lookAtZ look at vector
	/// \param upX, upY, upZ up vector
	//
	void ESUTIL_API
		esMatrixLookAt ( ESMatrix *result,
				float posX, float posY, float posZ,
				float lookAtX, float lookAtY, float lookAtZ,
				float upX, float upY, float upZ );
#ifdef __cplusplus
}
#endif
#endif // ESUTIL_H
