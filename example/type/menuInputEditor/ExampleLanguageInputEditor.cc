/**
 * @file
 * ExampleLanguageInputEditor implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include "ExampleLanguageInputEditor.hh"

extern "C"
{
	#include "ExampleLanguage.h"
}

/**
 * Constructs a ExampleLanguageInputEditor object.
 */
VuoInputEditor *ExampleLanguageInputEditorFactory::newInputEditor()
{
	return new ExampleLanguageInputEditor();
}

/**
 * Creates the tree that models the menu.
 */
VuoInputEditorMenuItem * ExampleLanguageInputEditor::setUpMenuTree()
{
	VuoInputEditorMenuItem *rootMenuItem = new VuoInputEditorMenuItem("root");

	rootMenuItem->addItem(new VuoInputEditorMenuItem(ExampleLanguage_summaryFromValue(ExampleLanguage_English),
													 ExampleLanguage_jsonFromValue(ExampleLanguage_English)));
	rootMenuItem->addItem(new VuoInputEditorMenuItem(ExampleLanguage_summaryFromValue(ExampleLanguage_Spanish),
													 ExampleLanguage_jsonFromValue(ExampleLanguage_Spanish)));
	rootMenuItem->addItem(new VuoInputEditorMenuItem(ExampleLanguage_summaryFromValue(ExampleLanguage_Mandarin),
													 ExampleLanguage_jsonFromValue(ExampleLanguage_Mandarin)));

	return rootMenuItem;
}
