/**
 * @file
 * VuoInputEditorHorizontalAlignment interface.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#ifndef VUOINPUTEDITORHORIZONTALALIGNMENT_HH
#define VUOINPUTEDITORHORIZONTALALIGNMENT_HH

#include "VuoInputEditorWithMenu.hh"

/**
 * A VuoInputEditorHorizontalAlignment factory.
 */
class VuoInputEditorHorizontalAlignmentFactory : public VuoInputEditorFactory
{
   Q_OBJECT
   Q_PLUGIN_METADATA(IID "org.vuo.inputEditor" FILE "VuoInputEditorHorizontalAlignment.json")
   Q_INTERFACES(VuoInputEditorFactory)

public:
   virtual VuoInputEditor * newInputEditor(void);
};

/**
 * An input editor that displays a menu of VuoLeapPointableType values.
 */
class VuoInputEditorHorizontalAlignment : public VuoInputEditorWithMenu
{
	Q_OBJECT

public:
	VuoInputEditorMenuItem * setUpMenuTree();
};

#endif // VUOINPUTEDITORHORIZONTALALIGNMENT_HH
