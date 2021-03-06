/**
 * @file
 * VuoVertices implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "VuoVertices.h"


/// @{
#ifdef VUO_COMPILER
VuoModuleMetadata({
					 "title" : "Scene Vertices",
					 "description" : "Vertices representing a 3D Object.",
					 "keywords" : [ "mesh", "vertex" ],
					 "version" : "1.0.0",
					 "dependencies" : [
						 "c",
						 "json"
					 ]
				 });
#endif
/// @}

/**
 * Allocates and registers the vertex (position, normal, ...) and element arrays.
 */
VuoVertices VuoVertices_alloc(unsigned int vertexCount, unsigned int elementCount)
{
	VuoVertices v;

	v.vertexCount = vertexCount;

	v.positions = (VuoPoint4d *)malloc(sizeof(VuoPoint4d)*v.vertexCount);
	VuoRegister(v.positions, free);

	v.normals = (VuoPoint4d *)malloc(sizeof(VuoPoint4d)*v.vertexCount);
	VuoRegister(v.normals, free);

	v.tangents = (VuoPoint4d *)malloc(sizeof(VuoPoint4d)*v.vertexCount);
	VuoRegister(v.tangents, free);

	v.bitangents = (VuoPoint4d *)malloc(sizeof(VuoPoint4d)*v.vertexCount);
	VuoRegister(v.bitangents, free);

	v.textureCoordinates = (VuoPoint4d *)malloc(sizeof(VuoPoint4d)*v.vertexCount);
	VuoRegister(v.textureCoordinates, free);

	v.elementCount = elementCount;

	v.elements = (unsigned int *)malloc(sizeof(unsigned int)*v.elementCount);
	VuoRegister(v.elements, free);

	return v;
}


/**
 * Returns a quad with dimensions 1x1, on the XY plane, centered at the origin.
 */
VuoVertices VuoVertices_getQuad(void)
{
	VuoVertices vertices = VuoVertices_alloc(4, 4);

	// Positions
	{
		vertices.positions[0] = VuoPoint4d_make(-.5,-.5,0,1);
		vertices.positions[1] = VuoPoint4d_make( .5,-.5,0,1);
		vertices.positions[2] = VuoPoint4d_make(-.5, .5,0,1);
		vertices.positions[3] = VuoPoint4d_make( .5, .5,0,1);
	}

	// Normals
	{
		for (int i=0; i<vertices.vertexCount; ++i)
			vertices.normals[i] = VuoPoint4d_make(0,0,1,1);
	}

	// Tangents
	{
		for (int i=0; i<vertices.vertexCount; ++i)
			vertices.tangents[i] = VuoPoint4d_make(1,0,0,1);
	}

	// Bitangents
	{
		for (int i=0; i<vertices.vertexCount; ++i)
			vertices.bitangents[i] = VuoPoint4d_make(0,1,0,1);
	}

	// Texture Coordinates
	{
		vertices.textureCoordinates[0] = VuoPoint4d_make(0,0,0,1);
		vertices.textureCoordinates[1] = VuoPoint4d_make(1,0,0,1);
		vertices.textureCoordinates[2] = VuoPoint4d_make(0,1,0,1);
		vertices.textureCoordinates[3] = VuoPoint4d_make(1,1,0,1);
	}

	// Triangle Strip elements
	vertices.elementAssemblyMethod = VuoVertices_TriangleStrip;
	vertices.elements[0] = 0;
	vertices.elements[1] = 1;
	vertices.elements[2] = 2;
	vertices.elements[3] = 3;

	return vertices;
}

/**
 * Returns a quad with dimensions 1x1, on the XY plane, centered at the origin.
 *
 * The quad consists of only positions and texture coordinates (without normals, tangents, or bitangents).
 */
VuoVertices VuoVertices_getQuadWithoutNormals(void)
{
	VuoVertices v;

	v.vertexCount = 4;

	v.positions = (VuoPoint4d *)malloc(sizeof(VuoPoint4d)*v.vertexCount);
	VuoRegister(v.positions, free);
	{
		v.positions[0] = VuoPoint4d_make(-.5,-.5,0,1);
		v.positions[1] = VuoPoint4d_make( .5,-.5,0,1);
		v.positions[2] = VuoPoint4d_make(-.5, .5,0,1);
		v.positions[3] = VuoPoint4d_make( .5, .5,0,1);
	}

	v.normals = NULL;
	v.tangents = NULL;
	v.bitangents = NULL;

	v.textureCoordinates = (VuoPoint4d *)malloc(sizeof(VuoPoint4d)*v.vertexCount);
	VuoRegister(v.textureCoordinates, free);
	{
		v.textureCoordinates[0] = VuoPoint4d_make(0,0,0,1);
		v.textureCoordinates[1] = VuoPoint4d_make(1,0,0,1);
		v.textureCoordinates[2] = VuoPoint4d_make(0,1,0,1);
		v.textureCoordinates[3] = VuoPoint4d_make(1,1,0,1);
	}

	v.elementCount = 4;
	v.elements = (unsigned int *)malloc(sizeof(unsigned int)*v.elementCount);
	VuoRegister(v.elements, free);
	v.elementAssemblyMethod = VuoVertices_TriangleStrip;
	v.elements[0] = 0;
	v.elements[1] = 1;
	v.elements[2] = 2;
	v.elements[3] = 3;

	return v;
}

/**
 * Returns an equilateral triangle with bottom edge length 1, pointing upward on the XY plane, centered at the origin.
 */
VuoVertices VuoVertices_getEquilateralTriangle(void)
{
	VuoVertices vertices = VuoVertices_alloc(3, 3);

	// Positions
	float yOffset = -sin(M_PI/3.)/2.;
	vertices.positions[0] = VuoPoint4d_make(0,   yOffset+sin(M_PI/3.), 0, 1);
	vertices.positions[1] = VuoPoint4d_make(-.5, yOffset, 0, 1);
	vertices.positions[2] = VuoPoint4d_make( .5, yOffset, 0, 1);

	// Normals
	for (int i=0; i<vertices.vertexCount; ++i)
		vertices.normals[i] = VuoPoint4d_make(0,0,1,1);

	// Tangents
	for (int i=0; i<vertices.vertexCount; ++i)
		vertices.tangents[i] = VuoPoint4d_make(1,0,0,1);

	// Bitangents
	for (int i=0; i<vertices.vertexCount; ++i)
		vertices.bitangents[i] = VuoPoint4d_make(0,1,0,1);

	// Texture Coordinates
	vertices.textureCoordinates[0] = VuoPoint4d_make(.5,1,0,1);
	vertices.textureCoordinates[1] = VuoPoint4d_make(0,0,0,1);
	vertices.textureCoordinates[2] = VuoPoint4d_make(1,0,0,1);

	// Triangle Strip elements
	vertices.elementAssemblyMethod = VuoVertices_TriangleStrip;
	vertices.elements[0] = 0;
	vertices.elements[1] = 1;
	vertices.elements[2] = 2;

	return vertices;
}

/**
 * Returns a VuoVertices consisting of the given positions and element assembly method.
 * Its normals, tangents, bitangents, and texture coordinates are all null.
 */
VuoVertices VuoVertices_makeFrom2dPoints(VuoList_VuoPoint2d positions, VuoVertices_ElementAssemblyMethod elementAssemblyMethod)
{
	unsigned long count = VuoListGetCount_VuoPoint2d(positions);
	VuoVertices vertices;

	vertices.vertexCount = count;
	vertices.positions = (VuoPoint4d *)malloc(sizeof(VuoPoint4d)*count);
	VuoRegister(vertices.positions, free);

	vertices.elementCount = count;
	vertices.elements = (unsigned int *)malloc(sizeof(unsigned int)*count);
	VuoRegister(vertices.elements, free);

	for (unsigned long i = 0; i < count; ++i)
	{
		VuoPoint2d xy = VuoListGetValueAtIndex_VuoPoint2d(positions, i+1);
		vertices.positions[i] = VuoPoint4d_make(xy.x, xy.y, 0, 1);
		vertices.elements[i] = i;
	}

	vertices.elementAssemblyMethod = elementAssemblyMethod;

	vertices.normals = NULL;
	vertices.tangents = NULL;
	vertices.bitangents = NULL;
	vertices.textureCoordinates = NULL;

	return vertices;
}

/**
 * Returns a VuoVertices consisting of the given positions and element assembly method.
 * Its normals, tangents, bitangents, and texture coordinates are all null.
 */
VuoVertices VuoVertices_makeFrom3dPoints(VuoList_VuoPoint3d positions, VuoVertices_ElementAssemblyMethod elementAssemblyMethod)
{
	unsigned long count = VuoListGetCount_VuoPoint3d(positions);
	VuoVertices vertices;

	vertices.vertexCount = count;
	vertices.positions = (VuoPoint4d *)malloc(sizeof(VuoPoint4d)*count);
	VuoRegister(vertices.positions, free);

	vertices.elementCount = count;
	vertices.elements = (unsigned int *)malloc(sizeof(unsigned int)*count);
	VuoRegister(vertices.elements, free);

	for (unsigned long i = 0; i < count; ++i)
	{
		VuoPoint3d xyz = VuoListGetValueAtIndex_VuoPoint3d(positions, i+1);
		vertices.positions[i] = VuoPoint4d_make(xyz.x, xyz.y, xyz.z, 1);
		vertices.elements[i] = i;
	}

	vertices.elementAssemblyMethod = elementAssemblyMethod;

	vertices.normals = NULL;
	vertices.tangents = NULL;
	vertices.bitangents = NULL;
	vertices.textureCoordinates = NULL;

	return vertices;
}

/**
 * @ingroup VuoVertices
 * Returns the @c VuoVertices_ElementAssemblyMethod corresponding with the string @c elementAssemblyMethodString.  If none matches, returns VuoVertices_IndividualTriangles.
 */
VuoVertices_ElementAssemblyMethod VuoVertices_elementAssemblyMethodFromCString(const char * elementAssemblyMethodString)
{
	if (strcmp(elementAssemblyMethodString,"individualTriangles")==0)
		return VuoVertices_IndividualTriangles;
	else if (strcmp(elementAssemblyMethodString,"triangleStrip")==0)
		return VuoVertices_TriangleStrip;
	else if (strcmp(elementAssemblyMethodString,"triangleFan")==0)
		return VuoVertices_TriangleFan;
	else if (strcmp(elementAssemblyMethodString,"individualLines")==0)
		return VuoVertices_IndividualLines;
	else if (strcmp(elementAssemblyMethodString,"lineStrip")==0)
		return VuoVertices_LineStrip;
	else if (strcmp(elementAssemblyMethodString,"points")==0)
		return VuoVertices_Points;

	return VuoVertices_IndividualTriangles;
}

/**
 * @ingroup VuoVertices
 * Returns a string constant representing @c elementAssemblyMethod.
 */
const char * VuoVertices_cStringForElementAssemblyMethod(VuoVertices_ElementAssemblyMethod elementAssemblyMethod)
{
	switch (elementAssemblyMethod)
	{
		case VuoVertices_IndividualTriangles:
			return "individualTriangles";
		case VuoVertices_TriangleStrip:
			return "triangleStrip";
		case VuoVertices_TriangleFan:
			return "triangleFan";
		case VuoVertices_IndividualLines:
			return "individualLines";
		case VuoVertices_LineStrip:
			return "lineStrip";
		case VuoVertices_Points:
			return "points";
	}
	return "(unknown element assembly method)";
}

/**
 * Helper method for @c VuoVertices_valueFromJson.
 * Turns the JSON array @c arrayName in object @c js into a C array of @c VuoPoint4ds.
 * If no array @c arrayName exists, initializes all elements to the zero point.
 */
void VuoVertices_verticesWithJsonArray(json_object *js, const char *arrayName, unsigned int expectedVertexCount, VuoPoint4d *vertices)
{
	json_object * o = NULL;
	if (!json_object_object_get_ex(js, arrayName, &o))
	{
		for (unsigned int i=0; i<expectedVertexCount; ++i)
			vertices[i] = (VuoPoint4d){0,0,0,0};
		return;
	}

	unsigned int vertexCount = json_object_array_length(o);
	if (!vertexCount)
		fprintf(stderr, "VuoVertices_verticesWithJsonArray() Error: Couldn't find any vertices for %s.\n", arrayName);
	if (vertexCount != expectedVertexCount)
		fprintf(stderr, "VuoVertices_verticesWithJsonArray() Error: Expected %u vertices, found %u.\n", expectedVertexCount, vertexCount);

	for (unsigned int i=0; i<expectedVertexCount; ++i)
	{
		json_object * jsp = json_object_array_get_idx(o, i);
		int dimensions = json_object_array_length(jsp);
		VuoPoint4d p;
		p.x = dimensions>0 ? json_object_get_double(json_object_array_get_idx(jsp, 0)) : 0;
		p.y = dimensions>1 ? json_object_get_double(json_object_array_get_idx(jsp, 1)) : 0;
		p.z = dimensions>2 ? json_object_get_double(json_object_array_get_idx(jsp, 2)) : 0;
		p.w = dimensions>3 ? json_object_get_double(json_object_array_get_idx(jsp, 3)) : 0;
		vertices[i] = p;
	}
}

/**
 * @ingroup VuoVertices
 * Decodes the JSON object @c js to create a new value.
 *
 * @param js A JSON object containing:
 *    - a @c positions array
 *    - an optional @c normals array
 *    - an optional @c tangents array
 *    - an optional @c bitangents array
 *    - an optional @c textureCoordinates array
 *    - an @c elementAssemblyMethod
 *    - an array of @c element indexes into the vertex array.
 *
 * For efficiency, the arrays may have been serialized as pointers to C arrays.
 * In that case, values for @c vertexCount and @c elementCount are required.
 *
 * @eg{
 *	{
 *		"positions": [
 *			[-0.6, -0.5],
 *			[ 0.5, -0.5],
 *			[-0.5,  0.5],
 *			[ 0.5,  0.5]
 *		],
 *		"elementAssemblyMethod": "triangleFan",
 *		"elements": [
 *			0,
 *			1,
 *			2,
 *			3
 *		]
 *	}
 * }
 */
VuoVertices VuoVertices_valueFromJson(json_object * js)
{
	VuoVertices v;

	// Defaults, in case parsing fails
	v.vertexCount = 0;
	v.positions = 0;
	v.normals = 0;
	v.tangents = 0;
	v.bitangents = 0;
	v.textureCoordinates = 0;
	v.elementCount = 0;
	v.elements = 0;
	v.elementAssemblyMethod = VuoVertices_IndividualTriangles;

	// Parse the initializer
	json_object * o = NULL;

	if (json_object_object_get_ex(js, "positions", &o))
	{
		if (json_object_is_type(o, json_type_array))
			v.vertexCount = json_object_array_length(o);
		else
		{
			if (json_object_object_get_ex(js, "vertexCount", &o))
				v.vertexCount = json_object_get_int64(o);
			else
				return v;
		}
	}
	else
		return v;

	if (json_object_object_get_ex(js, "elements", &o))
	{
		if (json_object_is_type(o, json_type_array))
			v.elementCount = json_object_array_length(o);
		else
		{
			if (json_object_object_get_ex(js, "elementCount", &o))
				v.elementCount = json_object_get_int64(o);
			else
				return v;
		}
	}

	// If "elements" is a JSON array, assume all other values are JSON arrays.
	if (json_object_is_type(o, json_type_array))
	{
		v = VuoVertices_alloc(v.vertexCount, v.elementCount);

		for (unsigned int i=0; i<v.elementCount; ++i)
			v.elements[i] = json_object_get_int64(json_object_array_get_idx(o, i));

		VuoVertices_verticesWithJsonArray(js, "positions", v.vertexCount, v.positions);
		VuoVertices_verticesWithJsonArray(js, "normals", v.vertexCount, v.normals);
		VuoVertices_verticesWithJsonArray(js, "tangents", v.vertexCount, v.tangents);
		VuoVertices_verticesWithJsonArray(js, "bitangents", v.vertexCount, v.bitangents);
		VuoVertices_verticesWithJsonArray(js, "textureCoordinates", v.vertexCount, v.textureCoordinates);
	}
	else
	{
		json_object_object_get_ex(js, "elements", &o);
		v.elements = (unsigned int *)json_object_get_int64(o);

		json_object_object_get_ex(js, "positions", &o);
		v.positions = (VuoPoint4d *)json_object_get_int64(o);

		json_object_object_get_ex(js, "normals", &o);
		v.normals = (VuoPoint4d *)json_object_get_int64(o);

		json_object_object_get_ex(js, "tangents", &o);
		v.tangents = (VuoPoint4d *)json_object_get_int64(o);

		json_object_object_get_ex(js, "bitangents", &o);
		v.bitangents = (VuoPoint4d *)json_object_get_int64(o);

		json_object_object_get_ex(js, "textureCoordinates", &o);
		v.textureCoordinates = (VuoPoint4d *)json_object_get_int64(o);
	}

	if (json_object_object_get_ex(js, "elementAssemblyMethod", &o))
		v.elementAssemblyMethod = VuoVertices_elementAssemblyMethodFromCString(json_object_get_string(o));

	return v;
}

/**
 * Helper method for @c VuoVertices_jsonFromValue.
 * Turns a C array of @c VuoPoint4ds into a @c json_object array.
 */
json_object *VuoVertices_jsonArrayWithVertices(VuoPoint4d *vertices, unsigned int vertexCount)
{
	json_object * o = json_object_new_array();
	for(unsigned int i=0; i<vertexCount; ++i)
	{
		json_object * v = json_object_new_array();
		json_object_array_add(v,json_object_new_double(vertices[i].x));
		json_object_array_add(v,json_object_new_double(vertices[i].y));
		json_object_array_add(v,json_object_new_double(vertices[i].z));
		json_object_array_add(v,json_object_new_double(vertices[i].w));
		json_object_array_add(o,v);
	}
	return o;
}

/**
 * @ingroup VuoVertices
 * Encodes @c value as a JSON object.
 */
json_object * VuoVertices_jsonFromValue(const VuoVertices value)
{
	json_object * js = json_object_new_object();

	json_object_object_add(js, "vertexCount", json_object_new_int64(value.vertexCount));
	json_object_object_add(js, "elementCount", json_object_new_int64(value.elementCount));

	/// @todo create an interprocess-serializer with the commented-out function calls
	if (value.positions)
//		json_object_object_add(js, "positions", VuoVertices_jsonArrayWithVertices(value.positions, value.vertexCount));
		json_object_object_add(js, "positions", json_object_new_int64((int64_t)value.positions));

	if (value.normals)
//		json_object_object_add(js, "normals", VuoVertices_jsonArrayWithVertices(value.normals, value.vertexCount));
		json_object_object_add(js, "normals", json_object_new_int64((int64_t)value.normals));

	if (value.tangents)
//		json_object_object_add(js, "tangents", VuoVertices_jsonArrayWithVertices(value.tangents, value.vertexCount));
		json_object_object_add(js, "tangents", json_object_new_int64((int64_t)value.tangents));

	if (value.bitangents)
//		json_object_object_add(js, "bitangents", VuoVertices_jsonArrayWithVertices(value.bitangents, value.vertexCount));
		json_object_object_add(js, "bitangents", json_object_new_int64((int64_t)value.bitangents));

	if (value.textureCoordinates)
//		json_object_object_add(js, "textureCoordinates", VuoVertices_jsonArrayWithVertices(value.textureCoordinates, value.vertexCount));
		json_object_object_add(js, "textureCoordinates", json_object_new_int64((int64_t)value.textureCoordinates));

	{
		json_object * o = json_object_new_string(VuoVertices_cStringForElementAssemblyMethod(value.elementAssemblyMethod));
		json_object_object_add(js, "elementAssemblyMethod", o);
	}

	{
//		json_object * o = json_object_new_array();
//		for(unsigned int i=0; i<value.elementCount; ++i)
//			json_object_array_add(o,json_object_new_int64(value.elements[i]));
//		json_object_object_add(js, "elements", o);
		json_object_object_add(js, "elements", json_object_new_int64((int64_t)value.elements));
	}

	return js;
}

/**
 * @ingroup VuoVertices
 * A brief summary of the contents of this mesh.
 *
 * @eg{4 vertices in a fan of 2 triangles
 * with first position (0,0,0,0)}
 * @eg{8 vertices, 12 triangles
 * with first position (-0.5,-0.5,-0.5,0)}
 */
char * VuoVertices_summaryFromValue(const VuoVertices value)
{
	if (value.vertexCount == 0)
	{
		char * zero = strdup("0 vertices");
		return zero;
	}

	const char * format = "%lu %s%s%lu %s%s<br>with first position (%g, %g, %g, %g)";

	unsigned long objectCount = 0;
	const char * objectString = "";
	const char * assemblyMethod = " (unknown element assembly method)";
	if (value.elementAssemblyMethod == VuoVertices_IndividualTriangles)
	{
		assemblyMethod = ", ";
		objectCount = value.elementCount/3;
		objectString = "triangle";
		/// @todo Report if value.elementCount isn't a multiple of 3.
	}
	else if (value.elementAssemblyMethod == VuoVertices_TriangleStrip)
	{
		assemblyMethod = " in a strip of ";
		objectCount = value.elementCount-2;
		objectString = "triangle";
	}
	else if (value.elementAssemblyMethod == VuoVertices_TriangleFan)
	{
		assemblyMethod = " in a fan of ";
		objectCount = value.elementCount-2;
		objectString = "triangle";
	}
	else if (value.elementAssemblyMethod == VuoVertices_IndividualLines)
	{
		assemblyMethod = ", ";
		objectCount = value.elementCount/2;
		objectString = "line";
	}
	else if (value.elementAssemblyMethod == VuoVertices_LineStrip)
	{
		assemblyMethod = " in a strip of ";
		objectCount = value.elementCount-1;
		objectString = "line";
	}
	else if (value.elementAssemblyMethod == VuoVertices_Points)
	{
		assemblyMethod = ", ";
		objectCount = value.elementCount;
		objectString = "point";
	}

	const char * vertexCountString = value.vertexCount==1 ? "vertex" : "vertices";
	const char * objectStringPlural = objectCount==1 ? "" : "s";
	VuoPoint4d p = value.positions[0];

	int size = snprintf(NULL,0,format,value.vertexCount,vertexCountString,assemblyMethod,objectCount,objectString,objectStringPlural,p.x,p.y,p.z,p.w);
	char * valueAsString = (char *)malloc(size+1);
	snprintf(valueAsString,size+1,format,value.vertexCount,vertexCountString,assemblyMethod,objectCount,objectString,objectStringPlural,p.x,p.y,p.z,p.w);
	return valueAsString;
}
