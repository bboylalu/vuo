﻿/**
 * @file
 * VuoCompilerPortClass implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the GNU Lesser General Public License (LGPL) version 2 or later.
 * For more information, see http://vuo.org/license.
 */

#include "VuoCompilerPortClass.hh"


/**
 * Creates a port type and creates its corresponding base @c VuoPortClass.
 */
VuoCompilerPortClass::VuoCompilerPortClass(string name, VuoPortClass::PortType portType, Type *type) :
	VuoCompilerNodeArgumentClass(name, portType, type)
{
}
