/**
 * @file
 * VuoType implementation.
 *
 * @copyright Copyright © 2012–2013 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the GNU Lesser General Public License (LGPL) version 2 or later.
 * For more information, see http://vuo.org/license.
 */

#include "VuoType.hh"

/**
 * Creats a type.
 *
 * @param typeName A unique (across all types and node classes) name for this type.
 */
VuoType::VuoType(string typeName)
	: VuoBase<VuoCompilerType,void>("VuoType"),
	  VuoModule(typeName)
{
}