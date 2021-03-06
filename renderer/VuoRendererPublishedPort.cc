/**
 * @file
 * VuoRendererPublishedPort implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the GNU Lesser General Public License (LGPL) version 2 or later.
 * For more information, see http://vuo.org/license.
 */

#include "VuoPortClass.hh"
#include "VuoGenericType.hh"
#include "VuoCompilerInputEventPortClass.hh"
#include "VuoCompilerOutputEventPortClass.hh"
#include "VuoCompilerPublishedPort.hh"
#include "VuoCompilerPortClass.hh"
#include "VuoRendererPublishedPort.hh"
#include "VuoRendererPort.hh"

/**
 * Creates a published input or output port.
 *
 * @param basePublishedPort The base for which this renderer detail is to be created.
 */
VuoRendererPublishedPort::VuoRendererPublishedPort(VuoPublishedPort *basePublishedPort)
	: VuoBaseDetail<VuoPublishedPort>("VuoRendererPublishedPort", basePublishedPort)
{
	getBase()->setRenderer(this);
	this->globalPos = QPointF();

	// We can't use the VuoPseudoPort for rendering purposes because it won't necessarily have the type that we
	// want the rendering to reflect. (e.g., published input pseudoports are always event-only trigger ports.)
	// Create a new VuoRendererPort to represent this published port.
	this->portRepresentation = createPortRepresentation(getBase()->getName(), getBase()->getType(), getBase()->getInput());
	this->portRepresentation->setProxyPublishedSidebarPort(this);

	// The VuoPsuedoPort must nevertheless have its own associated VuoRendererPort -- not because it will
	// be rendered (it will not), but so that cables connected to it will have a way to access this VuoPublishedPort.
	VuoPort *pseudoPort = getBase()->getCompiler()->getVuoPseudoPort();
	pseudoPort->setRenderer(new VuoRendererPort(pseudoPort, NULL, getBase()->getInput(), false, false, false));
	pseudoPort->getRenderer()->setProxyPublishedSidebarPort(this);

	setFlag(QGraphicsItem::ItemIsSelectable, true);
	updateNameRect();
}

/**
 * Creates and returns a VuoRendererPort to represent a published port within the sidebar.
 *
 * @param name The name of the published port.
 * @param type The data type of the published port.
 * @param isPublishedInput A boolean indicating whether the published port is a published input, as
 * opposed to a published output.
 */
VuoRendererPort * VuoRendererPublishedPort::createPortRepresentation(string name, VuoType *type, bool isPublishedInput)
{
	VuoCompilerPort *portRepresentationCompiler;
	if (isPublishedInput)
	{
		VuoCompilerOutputEventPortClass *portRepresentationOutputCompilerClass = new VuoCompilerOutputEventPortClass(name);

		if (type)
		{
			portRepresentationOutputCompilerClass->setDataClass(new VuoCompilerOutputDataClass(name, NULL));
			portRepresentationOutputCompilerClass->setDataVuoType(type);
		}

		portRepresentationCompiler = portRepresentationOutputCompilerClass->newPort();
	}
	else
	{
		VuoCompilerInputEventPortClass *portRepresentationInputCompilerClass = new VuoCompilerInputEventPortClass(name);

		if (type)
		{
			portRepresentationInputCompilerClass->setDataClass(new VuoCompilerInputDataClass(name, NULL, false));
			portRepresentationInputCompilerClass->setDataVuoType(type);
		}
		portRepresentationCompiler = portRepresentationInputCompilerClass->newPort();
	}

	VuoRendererPort *portRepresentation = new VuoRendererPort(
		portRepresentationCompiler->getBase(),
		NULL,
		// "Published input" ports are *output* ports within the published input node,
		// and vice versa.
		isPublishedInput,
		false,
		false,
		false
		);

	return portRepresentation;
}

/**
 * Draws the published port on @c painter.
 */
void VuoRendererPublishedPort::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	portRepresentation->paint(painter, option, widget);
}

/**
 * Returns a rectangle containing the rendered published port.
 */
QRectF VuoRendererPublishedPort::boundingRect(void) const
{
	return portRepresentation->boundingRect();
}

/**
 * Sets the published port's name.
 */
void VuoRendererPublishedPort::setName(string name)
{
	getBase()->setName(name);
	updateNameRect();
}

/**
 * Returns a boolean indicating whether a new @c internalPort may be
 * attached to/from this externally visible published port without
 * displacing any currently connected internal data ports.
 */
bool VuoRendererPublishedPort::canAccommodateInternalPort(VuoRendererPort *internalPort)
{
	if (! isCompatibleAliasWithSpecializationForInternalPort(internalPort))
		return false;

	// If this is an output port already acting as an alias for an internal port
	// carrying data, it cannot accommodate another connected data port.
	if (getBase()->getOutput() && internalPort->getDataType())
	{
		foreach (VuoPort *connectedPort, getBase()->getConnectedPorts())
			if (connectedPort->getRenderer()->getDataType())
				return false;
	}

	return true;
}

/**
 * Returns a boolean indicating whether there may be a cable
 * attached directly between this externally visible published port
 * and the input @c internalPort, taking into account
 * the respective port types (input vs. output; event-only vs.
 * event+data; respective data types).
 *
 * If the connection would require one or both ports to be specialized, returns false.
 * (But see @c VuoRendererPublishedPort::isCompatibleAliasWithSpecializationForInternalPort(...).)
 */
bool VuoRendererPublishedPort::isCompatibleAliasWithoutSpecializationForInternalPort(VuoRendererPort *internalPort)
{
	// Temporarily disallow direct cable connections between published inputs and published outputs.
	// @todo: Allow for https://b33p.net/kosada/node/7756 .
	if (internalPort->getProxyPublishedSidebarPort())
		return false;

	return (getBase()->getInput()? portRepresentation->canConnectDirectlyWithoutSpecializationTo(internalPort) :
								   internalPort->canConnectDirectlyWithoutSpecializationTo(portRepresentation));
}

/**
 * Returns a boolean indicating whether there may be a cable
 * attached directly between this externally visible published port
 * and the input @c internalPort, taking into account
 * the respective port types (input vs. output; event-only vs.
 * event+data; respective data types), and the possibility that one
 * port may be specialized in preparation for the connection.
 *
 * Convenience function for VuoRendererPublishedPort::isCompatibleAliasWithSpecializationForInternalPort(VuoRendererPort *internalPort,
 * VuoRendererPort **portToSpecialize, string &specializedTypeName), for use
 * when only the returned boolean and none of the other output parameter values are needed.
 */
bool VuoRendererPublishedPort::isCompatibleAliasWithSpecializationForInternalPort(VuoRendererPort *internalPort)
{
	VuoRendererPort *portToSpecialize = NULL;
	string specializedTypeName = "";

	return this->isCompatibleAliasWithSpecializationForInternalPort(internalPort, &portToSpecialize, specializedTypeName);
}

/**
 * Returns a boolean indicating whether there may be a cable
 * attached directly between this externally visible published port
 * and the input @c internalPort, taking into account
 * the respective port types (input vs. output; event-only vs.
 * event+data; respective data types), and the possibility that one
 * port may be specialized in preparation for the connection.
 */
bool VuoRendererPublishedPort::isCompatibleAliasWithSpecializationForInternalPort(VuoRendererPort *internalPort,
																				  VuoRendererPort **portToSpecialize,
																				  string &specializedTypeName)
{
	*portToSpecialize = NULL;
	specializedTypeName = "";

	if (this->isCompatibleAliasWithoutSpecializationForInternalPort(internalPort))
		return true;

	// Temporarily disallow direct cable connections between published inputs and published outputs.
	// @todo: Allow for https://b33p.net/kosada/node/7756 .
	if (internalPort->getProxyPublishedSidebarPort())
		return false;

	return (getBase()->getInput()? portRepresentation->canConnectDirectlyWithSpecializationTo(internalPort, portToSpecialize, specializedTypeName) :
								   internalPort->canConnectDirectlyWithSpecializationTo(portRepresentation, portToSpecialize, specializedTypeName));
}

/**
 * Returns a boolean indicating whether the @c otherExternalPort may be
 * merged with this one, taking into account
 * the respective port types (input vs. output; event-only vs.
 * event+data; respective data types), without
 * displacing any currently connected internal data ports.
 */
bool VuoRendererPublishedPort::canBeMergedWith(VuoPublishedPort *otherExternalPort)
{
	// For these two externally visible published ports to be eligible for merging, they must have the same type.
	if (! ((this->getBase()->getInput() == otherExternalPort->getInput()) &&
		   (this->getBase()->getType() == otherExternalPort->getType())))
		return false;

	// If they are input published ports and their types match, go ahead and merge them.
	if (this->getBase()->getInput())
		return true;

	// If they are output published ports and their types match, make sure they
	// have no more than one connected internal data port between the two of them.
	bool foundConnectedDataPort = false;
	set<VuoPort *> connectedPorts = getBase()->getConnectedPorts();
	for (set<VuoPort *>::iterator port = connectedPorts.begin(); (! foundConnectedDataPort) && (port != connectedPorts.end()); ++port)
	{
		if ((static_cast<VuoCompilerPortClass *>((*port)->getClass()->getCompiler()))->getDataVuoType())
			foundConnectedDataPort = true;
	}

	if (! foundConnectedDataPort)
		return true;

	else
	{
		set<VuoPort *> otherExternalPortConnectedPorts = otherExternalPort->getConnectedPorts();
		for (set<VuoPort *>::iterator port = otherExternalPortConnectedPorts.begin(); port != otherExternalPortConnectedPorts.end(); ++port)
		{
			if ((static_cast<VuoCompilerPortClass *>((*port)->getClass()->getCompiler()))->getDataVuoType())
				return false;
		}
	}

	return true;
}

/**
 * Adds the specified @c port to the list of internal ports for which this published port is an alias.
 */
void VuoRendererPublishedPort::addConnectedPort(VuoPort *port)
{
	getBase()->addConnectedPort(port);
	updateNameRect();
}

/**
 * Removes the specified @c port from the list of internal ports for which this published port is an alias.
 */
void VuoRendererPublishedPort::removeConnectedPort(VuoPort *port)
{
	getBase()->removeConnectedPort(port);
	updateNameRect();
}

/**
 * Returns the location of the published port within the published port sidebar,
 * in global coordinates.
 */
QPointF VuoRendererPublishedPort::getGlobalPos(void) const
{
	return globalPos;
}

/**
 * Sets the location of the published port within the published port sidebar,
 * in global coordinates.
 */
void VuoRendererPublishedPort::setGlobalPos(QPointF pos)
{
	this->globalPos = pos;
}

/**
 * Returns the VuoRendererPort that visually represents this published port.
 */
VuoRendererPort * VuoRendererPublishedPort::getPortRepresentation()
{
	return portRepresentation;
}

/**
 * Updates the cached bounding box of the published port's label within the published port sidebar.
 */
void VuoRendererPublishedPort::updateNameRect()
{
	portRepresentation->updateNameRect();
}
