/**
 * @file
 * VuoRendererPort implementation.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the GNU Lesser General Public License (LGPL) version 2 or later.
 * For more information, see http://vuo.org/license.
 */

#include "VuoStringUtilities.hh"

#include "VuoRendererComposition.hh"
#include "VuoRendererCable.hh"
#include "VuoRendererColors.hh"
#include "VuoRendererFonts.hh"
#include "VuoRendererPort.hh"
#include "VuoRendererTypecastPort.hh"
#include "VuoRendererNode.hh"
#include "VuoRendererMakeListNode.hh"
#include "VuoRendererSignaler.hh"

#include "VuoCompilerCable.hh"
#include "VuoCompilerNodeClass.hh"
#include "VuoCompilerMakeListNodeClass.hh"
#include "VuoCompilerNode.hh"
#include "VuoCompilerOutputEventPort.hh"
#include "VuoCompilerTriggerPort.hh"

#include "VuoGenericType.hh"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
	#include "json/json.h"
#pragma clang diagnostic pop
#include <sstream>

#include "VuoPort.hh"

const qreal VuoRendererPort::portRadius = VuoRendererFonts::thickPenWidth*0.3625;
const qreal VuoRendererPort::portSpacing = VuoRendererFonts::thickPenWidth*3.0/4.0;
const qreal VuoRendererPort::portContainerMargin = VuoRendererFonts::thickPenWidth / 6.;
const qreal VuoRendererPort::portInset = 1.4;
const qreal VuoRendererPort::constantFlagHeight = VuoRendererFonts::thickPenWidth*0.6;

/**
 * Creates a renderer detail for the specified base port.
 */
VuoRendererPort::VuoRendererPort(VuoPort * basePort, VuoRendererSignaler *signaler,
								 bool isOutput, bool isRefreshPort, bool isDonePort, bool isFunctionPort)
	: VuoBaseDetail<VuoPort>("VuoRendererPort standard", basePort)
{
	getBase()->setRenderer(this);

	this->signaler = signaler;

	this->isOutput = isOutput;
	this->isRefreshPort = isRefreshPort;
	this->isDonePort = isDonePort;
	this->isFunctionPort = isFunctionPort;
	this->isEligibleForDirectConnection = false;
	this->isEligibleForConnectionViaTypecast = false;
	this->isEligibleForSelection = false;
	setAnimated(false);
	this->typecastParentPort = NULL;
	this->proxyPublishedSidebarPort = NULL;
	resetTimeLastEventFired();

	const int maxAnimationsPerPort = 4;

	if (getBase()->getClass()->getPortType() == VuoPortClass::triggerPort)
	{
		for (int i = 0; i < maxAnimationsPerPort; ++i)
		{
			QGraphicsItemAnimation *animation = new QGraphicsItemAnimation();
			animation->setTimeLine(new QTimeLine(VuoRendererColors::activityAnimationFadeDuration));
			animations.push_back(animation);
		}
	}

	setFlag(QGraphicsItem::ItemIsFocusable, true);  // allow delivery of key events
	setAcceptHoverEvents(true);  // allow delivery of mouse-hover events
	updateNameRect();
	updateEnabledStatus();

	// Create a hybrid rect having the width of the port's inset rect and the customized
	// height of a constant flag, so that the constant flag has the desired height but
	// directly adjoins the inset port shape.
	QRectF portInnerRect = VuoRendererPort::getPortPath(VuoRendererPort::portInset).boundingRect();
	this->portHybridRect = QRectF(portInnerRect.x(),
								   -0.5*VuoRendererPort::constantFlagHeight,
								   portInnerRect.width(),
								   VuoRendererPort::constantFlagHeight);
	updatePortConstantPath();
}

/**
 * Calculates and updates the cached constant path for this port based on its current attributes.
 */
void VuoRendererPort::updatePortConstantPath()
{
	QString constantText = QString::fromUtf8(getConstantAsStringToRender().c_str());
	this->portConstantPath = getPortConstantPath(this->portHybridRect, constantText, &this->outsetPath);
	this->outsetMinusPortConstantPath = QPainterPath(this->outsetPath) - this->portConstantPath;
}

/**
 * Returns a path representing the frame of a port constant.
 * If @c outsetPath is not NULL, it is given a path that can be stroked with a pen of width 1 without overlapping the returned path.
 */
QPainterPath VuoRendererPort::getPortConstantPath(QRectF innerPortRect, QString text, QPainterPath *outsetPath, bool isTypecast)
{
	qreal portConstantTextMargin = VuoRendererFonts::thickPenWidth*3./20. + (isTypecast ? 2 : 0);
	qreal innerOffsetForTypecast = isTypecast ? 1. : 0.;
	qreal outerCornerRadius = VuoRendererFonts::thickPenWidth/8.0;
	qreal innerCornerRadius = outerCornerRadius - 1;

	QPainterPath p;
	QRectF textRect = getPortConstantTextRectForText(text);

	QPointF topLeftCorner(textRect.x() - portConstantTextMargin - (isTypecast ? 1. : 0.), innerPortRect.y() + innerOffsetForTypecast);
	QPointF topRightCorner(innerPortRect.x() - innerPortRect.width()/2 - portInset, innerPortRect.y() + innerOffsetForTypecast);
	QPointF rightPoint(innerPortRect.x() - portInset, innerPortRect.center().y());
	QPointF bottomRightCorner(innerPortRect.x() - innerPortRect.width()/2. - portInset, innerPortRect.bottom() - innerOffsetForTypecast);
	QPointF bottomLeftCorner(textRect.x() - portConstantTextMargin - (isTypecast ? 1. : 0.), innerPortRect.bottom() - innerOffsetForTypecast);

	p.moveTo(topRightCorner);
	p.lineTo(rightPoint);
	p.lineTo(bottomRightCorner);
	addRoundedCorner(p, true, bottomLeftCorner, innerCornerRadius, false, true);
	addRoundedCorner(p, true, topLeftCorner, innerCornerRadius, true, true);
	p.lineTo(topRightCorner);

	if (outsetPath)
	{
		QPointF outsetTopLeftCorner(textRect.x() - portConstantTextMargin - (isTypecast ? 1. : 0.) - 1., innerPortRect.y() - 1.);
		QPointF outsetTopRightCorner(innerPortRect.x() - innerPortRect.width()/2. - portInset + .2, innerPortRect.y() - 1.);
		QPointF outsetRightPoint(innerPortRect.x() + .2, innerPortRect.center().y());
		QPointF outsetBottomRightCorner(innerPortRect.x() - innerPortRect.width()/2. - portInset + .2, innerPortRect.bottom() + 1.);
		QPointF outsetBottomLeftCorner(textRect.x() - portConstantTextMargin - (isTypecast ? 1. : 0.) - 1., innerPortRect.bottom() + 1.);

		*outsetPath = QPainterPath();
		outsetPath->moveTo(outsetTopRightCorner);
		outsetPath->lineTo(outsetRightPoint);
		outsetPath->lineTo(outsetBottomRightCorner);
		addRoundedCorner(*outsetPath, true, outsetBottomLeftCorner, outerCornerRadius, false, true);
		addRoundedCorner(*outsetPath, true, outsetTopLeftCorner, outerCornerRadius, true, true);
		outsetPath->lineTo(outsetTopRightCorner);
	}

	return p;
}


/**
 * Returns a closed path representing the port's circle/triangle.  Does not include constant flag (see @c getPortConstantPath).
 */
QPainterPath VuoRendererPort::getPortPath(qreal inset) const
{
	bool carriesData = (getBase()->getClass()->hasCompiler() &&
						((VuoCompilerPortClass *)getBase()->getClass()->getCompiler())->getDataVuoType());

	return getPortPath(inset,
					   getBase()->getClass()->getPortType(),
					   getInput(),
					   carriesData
					   );
}

/**
 * Returns a closed path representing the circle/triangle for a port with type @c portType.
 * Does not include constant flag (see @c getPortConstantPath).
 */
QPainterPath VuoRendererPort::getPortPath(qreal inset,
										  VuoPortClass::PortType portType,
										  bool isInputPort,
										  bool carriesData
										 )
{
	QPainterPath p;
	QRectF outerPortRect = getPortRect();
	QRectF innerPortRect = outerPortRect.adjusted(inset,inset,-inset,-inset);

	switch (portType)
	{
		case VuoPortClass::notAPort:
			break;
		case VuoPortClass::dataAndEventPort:
			// Circle.
			p.addEllipse(innerPortRect);
			break;
		case VuoPortClass::eventOnlyPort:
		{
			// Triangle.

			// Align the port rect to integer pixels, so lines are sharp.
			outerPortRect = outerPortRect.toRect();

			// Move right 1 pixel, so the triangle's center-of-gravity better matches circular ports.
			outerPortRect = outerPortRect.adjusted(1,0,1,0);

			// Adjust the inset so that the amount of whitespace padding the triangle's
			// vertical left edge matches the amount of whitespace padding its diagonal edges.
			qreal h = outerPortRect.height();
			qreal w = outerPortRect.width();
			qreal triangleLeftInset = (inset*(w-0.5*h))/sqrt(pow(0.5*h,2)+pow(w,2));

			// Round the left inset to the nearest whole pixel to eliminate blur for input ports.
			if (isInputPort)
				triangleLeftInset = qRound(triangleLeftInset);

			qreal triangleVerticalInset = inset-(0.5*h/w)*(inset-triangleLeftInset);

			innerPortRect = outerPortRect.adjusted(triangleLeftInset, triangleVerticalInset, -inset, -triangleVerticalInset);

			p.moveTo(innerPortRect.right(), innerPortRect.center().y());
			p.lineTo(innerPortRect.topLeft());
			p.lineTo(innerPortRect.bottomLeft());
			p.closeSubpath();
			break;
		}
		case VuoPortClass::triggerPort:
			if (carriesData)
			{
				// Right half of circle...
				p.moveTo(innerPortRect.center().x(), innerPortRect.top());
				p.arcTo(innerPortRect,90,-180);
			}
			else
			{
				// Right tip of triangle...
				p.moveTo(innerPortRect.right(), innerPortRect.center().y());
			}

			// ...and left half explosion.
			float arcRadius = innerPortRect.width()/2.;
			float striationRadius = arcRadius*1.6;
			bool inner = carriesData;
			for (int i = (carriesData ? 1 : 2); i < (carriesData ? 10 : 9); ++i)
			{
				p.lineTo(cos((float)i*M_PI/10. + M_PI/2.) * (inner ? arcRadius : striationRadius),
						 sin((float)i*M_PI/10. + M_PI/2.) * (inner ? arcRadius : striationRadius));
				inner = !inner;
			}
			p.closeSubpath();
			break;
	}

	return p;
}


/**
 * Returns a path representing the glyph for function ports.
 */
QPainterPath VuoRendererPort::getFunctionPortGlyph(void) const
{
	// Started with the "f" glyph from Signika, then mirrored the top to the bottom using TouchDraw (see functionPort.t2d),
	// then converted the SVG points manually to QPainterPath statements.
	QPainterPath p;
	p.moveTo(4.43432694097478,7.85885865921973);
	p.cubicTo(4.54036026626481,8.63606869973077,4.74441887553019,9.56484019503971,4.75274533523895,10.1928227448389);
	p.cubicTo(4.72128346257729,11.5340886975457,4.07231883670452,11.9700122913261,3.0456930331924,11.9990782089261);
	p.cubicTo(2.48601155026458,12.0156314445099,1.53237634536436,11.6260438763205,0.942586240509103,11.5907867879313);
	p.cubicTo(0.691548665364582,12.0521443684896,0.443317511968114,12.628296671768,0.447553573494685,13.2469575591589);
	p.cubicTo(1.12910328803351,13.4954516035728,2.61869803131267,13.8462628915561,3.48793785656784,13.8437212530216);
	p.cubicTo(5.53069082864804,13.8377482873112,7.17526649001644,12.3276012820681,7.18981660610081,10.1534969961992);
	p.cubicTo(7.16594715465797,9.45320361017714,6.983790429683,8.62373810487953,6.88077734147409,7.85885865921973);
	p.lineTo(8.86491502201942,7.8528224278457);
	p.cubicTo(8.93434118668425,7.47692149366752,8.94681182167128,7.16303272511104,8.94430567454254,6.84815303891249);
	p.cubicTo(8.9422358996167,6.58810043794526,8.89028454897106,6.27550508860728,8.82579627591599,6.02535784537993);
	p.lineTo(6.71122119472517,6.09063125766457);
	p.cubicTo(6.61433216601297,5.38818431265963,6.54166550997983,4.6857373676547,6.54166550997983,4.05595804896617);
	p.cubicTo(6.54166550997983,2.74795371137328,7.21988917296796,2.02128576427494,8.33411415816619,2.02128576427494);
	p.cubicTo(9.01233782115432,2.02128576427494,9.78745097485828,2.19084109499369,10.4172298924888,2.3846201998277);
	p.cubicTo(10.7805640966619,1.99706199016,10.9985649887684,1.27039311905448,10.9985649887684,0.689059500581475);
	p.cubicTo(10.2476746697465,0.398390843330462,8.98811544847555,0.13194596019471,7.89811283595644,0.13194596019471);
	p.cubicTo(5.52432955349385,0.13194596019471,4.16788176551395,1.75483903312481,4.16788176551395,3.91062464434798);
	p.cubicTo(4.16788176551395,4.58884873924487,4.240548652549,5.31551761035038,4.31321553958377,6.06640840755649);
	p.lineTo(2.35121074464882,6.04218648145591);
	p.cubicTo(2.27854391536436,6.35707614080034,2.25432162931124,6.64774387404386,2.25432162931124,6.96263353338827);
	p.cubicTo(2.25432162931124,7.27752319273238,2.27854391536436,7.59241285207678,2.35121074464882,7.90730343542837);
	p.closeSubpath();

	// Center
	p.translate(-p.boundingRect().center());

	return p;
}

/**
 * Returns a rectangle encompassing the port's circle.
 */
QRectF VuoRendererPort::getPortRect(void)
{
	return QRectF(
		-portRadius,
		-portRadius,
		portRadius*2.0,
		portRadius*2.0
	);
}

/**
 * Returns the collapsed "Make List" node attached to this input port, or @c NULL if none.
 */
VuoRendererMakeListNode * VuoRendererPort::getAttachedInputDrawer(void) const
{
	if (! getInput())
		return NULL;

	vector<VuoCable *> inCables = getBase()->getConnectedCables(false);
	VuoCable *incomingDataCable = NULL;

	for (vector<VuoCable *>::iterator i = inCables.begin(); !incomingDataCable && (i != inCables.end()); ++i)
		if ((*i)->hasRenderer() && (*i)->getRenderer()->effectivelyCarriesData())
			incomingDataCable = *i;

	if (! incomingDataCable)
		return NULL;

	VuoNode *fromNode = incomingDataCable->getFromNode();

	if (fromNode && fromNode->hasRenderer() && dynamic_cast<VuoRendererMakeListNode *>(fromNode->getRenderer()))
		return (VuoRendererMakeListNode *)fromNode->getRenderer();

	return NULL;
}

/**
 * Returns a rect enclosing the string representation of the port's constant value.
 * If the port does not have a constant value, returns a null rect.
 */
QRectF VuoRendererPort::getPortConstantTextRect(void) const
{
	return (isConstant()?
				getPortConstantTextRectForText(QString::fromUtf8(getConstantAsStringToRender().c_str())) :
				QRectF());
}

/**
 * Returns a rect enclosing the specified @c text.
 */
QRectF VuoRendererPort::getPortConstantTextRectForText(QString text)
{
	qreal textWidth = QFontMetricsF(VuoRendererFonts::getSharedFonts()->nodePortTitleFont()).boundingRect(QRectF(0,0,0,0), Qt::TextIncludeTrailingSpaces, text).width()+1.0;
	QRectF textRect(
		-textWidth - getPortRect().width(),
		-VuoRendererFonts::thickPenWidth/3.0,
		textWidth,
		VuoRendererFonts::thickPenWidth
	);

	return textRect.toAlignedRect();
}


/**
 * Returns the cached bounding box of the port's label.
 */
QRectF VuoRendererPort::getNameRect() const
{
	return this->nameRect;
}

/**
 * Updates the cached bounding box of the port's label.
 */
void VuoRendererPort::updateNameRect()
{
	bool sidebarPaintMode = getProxyPublishedSidebarPort();
	QString text = QString::fromUtf8(sidebarPaintMode? getProxyPublishedSidebarPort()->getBase()->getName().c_str() :
													   getBase()->getClass()->getName().c_str());
	QFont font = VuoRendererFonts::getSharedFonts()->nodePortTitleFont();
	QSizeF textSize = QFontMetricsF(font).size(0,text);

	this->nameRect = QRectF(
		(isOutput? -VuoRendererFonts::thickPenWidth/2.0 - textSize.width() - 2. :
										 VuoRendererFonts::thickPenWidth/2.0
											 - VuoRendererFonts::getHorizontalOffset(font, text)
											 + (isOnDrawer() ? 1. : 3.)
										 ),
		-VuoRendererFonts::thickPenWidth/3.0,
		textSize.width(),
		textSize.height()
	);
}

/**
 * Returns a pointer to the node to which this port belongs in the underlying
 * Graphviz (.dot/.vuo) representation of the composition.
 *
 * For a given port, this value will never change.
 * For typecast ports, this method will always return the original parent typecast node,
 * regardless of whether the typecast is currently free-standing or collapsed.
 */
VuoRendererNode * VuoRendererPort::getUnderlyingParentNode(void) const
{
	VuoRendererNode *node;
	if (getTypecastParentPort())
		node = (((VuoRendererTypecastPort *)(getTypecastParentPort()))->getUncollapsedTypecastNode());
	else
		node = getRenderedParentNode();

	return node;
}

/**
 * Returns a pointer to the node currently rendered as this port's parent.
 *
 * For ports belonging to typecast nodes, this value will change depending
 * whether the typecast is currently free-standing (in which case that is the node that will
 * be returned) or collapsed (in which case the downstream node to which the typecast is
 * attached is the one that will be returned).
 */
VuoRendererNode * VuoRendererPort::getRenderedParentNode(void) const
{
	if (!parentItem())
		return NULL;

	if (!parentItem()->parentItem())
		return (VuoRendererNode *)(parentItem());

	return (VuoRendererNode *)(parentItem()->parentItem());
}

/**
 * Returns true if this port is part of a drawer (a collapsed Make List node).
 */
bool VuoRendererPort::isOnDrawer(void) const
{
	if (!parentItem() || !parentItem()->parentItem())
		return false;

	return (dynamic_cast<VuoRendererMakeListNode *>(parentItem()->parentItem()));
}

/**
 * Returns this port's typecast parent port, or NULL if it has none.
 */
VuoRendererPort * VuoRendererPort::getTypecastParentPort() const
{
	return typecastParentPort;
}

/**
 * Sets this port's typecast parent port.
 */
void VuoRendererPort::setTypecastParentPort(VuoRendererPort *typecastParentPort)
{
	this->typecastParentPort = typecastParentPort;
}

/**
 * Returns the bounding rectangle of this port (and its optional name and plug).
 */
QRectF VuoRendererPort::boundingRect(void) const
{
	VuoRendererNode *renderedParentNode = getRenderedParentNode();
	if (renderedParentNode && renderedParentNode->getProxyNode())
		return QRectF();

	QRectF r = getPortPath(1.5).boundingRect();

	if (portNameRenderingEnabled())
		r = r.united(getNameRect());

	if (isConstant())
		r = r.united(this->outsetPath.controlPointRect());

	// Antialiasing bleed
	r.adjust(-1,-1,1,1);

	return r.toAlignedRect();
}

/**
 * Returns the shape of the rendered port, for use in collision detection,
 * hit tests, and QGraphicsScene::items() functions.
 */
QPainterPath VuoRendererPort::shape() const
{
	QPainterPath p;
	p.addRect(boundingRect());
	return p;
}

/**
 * Paints the port's event wall or door.
 */
void VuoRendererPort::paintEventBarrier(QPainter *painter, VuoRendererColors *colors)
{
	bool sidebarPaintMode = getProxyPublishedSidebarPort();

	VuoPortClass::PortType type = getBase()->getClass()->getPortType();
	VuoPortClass::EventBlocking eventBlocking = getBase()->getClass()->getEventBlocking();

	if (
			!isAnimated &&
			((!isOutput && !sidebarPaintMode && eventBlocking != VuoPortClass::EventBlocking_None)
			|| (isOutput && type == VuoPortClass::triggerPort))
			)
	{
		QColor eventBlockingBarrierColor = (isAnimated? colors->animatedeventBlockingBarrier() : colors->eventBlockingBarrier());
		painter->setPen(QPen(eventBlockingBarrierColor, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

		if (type == VuoPortClass::dataAndEventPort)
		{
			// Circular port
			QRectF barrierRect = getPortRect().adjusted(-1.5,-1.5,1.5,1.5);

			if (eventBlocking == VuoPortClass::EventBlocking_Wall)
				painter->drawArc(barrierRect,40*16,-80*16);
			else // VuoPortClass::EventBlocking_Door
			{
				painter->drawArc(barrierRect,40*16,-15*16);
				painter->drawArc(barrierRect,-40*16,15*16);
			}
		}
		else if (type == VuoPortClass::eventOnlyPort)
		{
			// Triangular port
			QRectF barrierRect = getPortRect().adjusted(-3.5,-3.5,3.5,3.5);
			QLineF topLine = QLineF(barrierRect.topLeft(),QPointF(barrierRect.right(),barrierRect.center().y()));
			QLineF bottomLine = QLineF(barrierRect.bottomLeft(),QPointF(barrierRect.right(),barrierRect.center().y()));

			if (eventBlocking == VuoPortClass::EventBlocking_Wall)
			{
				QPainterPath p;
				p.moveTo(topLine.pointAt(0.65));
				p.lineTo(topLine.p2());
				p.lineTo(bottomLine.pointAt(0.65));
				painter->drawPath(p);
			}
			else // VuoPortClass::EventBlocking_Door
			{
				QPainterPath p;
				p.moveTo(topLine.pointAt(0.65));
				p.lineTo(topLine.pointAt(0.75));
				p.moveTo(bottomLine.pointAt(0.75));
				p.lineTo(bottomLine.pointAt(0.65));
				painter->drawPath(p);
			}
		}
		else if (type == VuoPortClass::triggerPort)
		{
			// Exploding port
			QRectF barrierRect = getPortRect().adjusted(-3,-3,3,3);
			painter->drawArc(barrierRect,145*16,70*16);
		}
	}

#if 0
	{
		// Draw the function port's glyph
		painter->save();
		painter->scale(.6,.6);
		painter->fillPath(getFunctionPortGlyph(), colors->nodeTitle());
		painter->restore();
	}
#endif
}

/**
 * Paints the port's label.
 */
void VuoRendererPort::paintPortName(QPainter *painter, VuoRendererColors *colors)
{
	bool sidebarPaintMode = getProxyPublishedSidebarPort();
	if (!portNameRenderingEnabled())
		return;

	string name = (sidebarPaintMode? getProxyPublishedSidebarPort()->getBase()->getName() : getBase()->getClass()->getName());

	painter->setPen(sidebarPaintMode && getProxyPublishedSidebarPort()->isSelected()? Qt::white : colors->portTitle());
	painter->setFont(VuoRendererFonts::getSharedFonts()->nodePortTitleFont());
	painter->drawText(getNameRect(), isOutput? Qt::AlignRight : Qt::AlignLeft, QString::fromStdString(name));
}

/**
 * Draws an input or output port (both standard ports and refresh/done/function ports).
 */
void VuoRendererPort::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	VuoRendererNode *renderedParentNode = getRenderedParentNode();
	if (renderedParentNode && renderedParentNode->getProxyNode())
		return;

	drawBoundingRect(painter);

	bool sidebarPaintMode = getProxyPublishedSidebarPort();

	bool isColorInverted = isRefreshPort || isDonePort || isFunctionPort;

	VuoRendererColors::SelectionType selectionType = ((renderedParentNode && renderedParentNode->isSelected() && !sidebarPaintMode)?
														  VuoRendererColors::directSelection :
														  VuoRendererColors::noSelection);

	bool isHovered = isEligibleForSelection;
	VuoRendererColors::HighlightType highlightType = (isEligibleForDirectConnection? VuoRendererColors::standardHighlight :
												(isEligibleForConnectionViaTypecast? VuoRendererColors::subtleHighlight :
																					 VuoRendererColors::noHighlight));

	VuoPortClass::PortType type = getBase()->getClass()->getPortType();
	bool isTriggerPort = (type == VuoPortClass::triggerPort);

	qint64 timeOfLastActivity =		((! getRenderActivity())? VuoRendererItem::notTrackingActivity :
									(isTriggerPort? timeLastEventFired :
									(renderedParentNode? renderedParentNode->getTimeLastExecutionEnded() :
									VuoRendererItem::notTrackingActivity)));

	VuoRendererColors *colors = new VuoRendererColors((renderedParentNode? renderedParentNode->getBase()->getTintColor() : VuoNode::TintNone),
													  selectionType,
													  isHovered,
													  highlightType,
													  timeOfLastActivity);

	VuoRendererColors *untintedColors = new VuoRendererColors(VuoNode::TintNone,
													  selectionType,
													  isHovered,
													  highlightType,
													  timeOfLastActivity);

	// Draw the port circle / constant flag
	QPainterPath portPath = getPortPath(VuoRendererPort::portInset);

	QBrush portBrush;

	if (isColorInverted)
		portBrush = colors->portTitlebarFill();
	else if (isAnimated)
		portBrush = colors->animatedPortFill();
	else if (sidebarPaintMode)
		portBrush = colors->publishedPortFill();
	else
		portBrush = colors->portFill();

	painter->fillPath(portPath, portBrush);

	if(isConstant())
	{
		QString constantText = QString::fromUtf8(getConstantAsStringToRender().c_str());
		QBrush constantFlagBackgroundBrush = colors->constantFill();
		QColor constantFlagBorderColor = colors->nodeFill();

		// Display a color swatch for VuoColor data.
		/// @todo Implement with input viewers (https://b33p.net/kosada/node/5700)
		bool isColorPort = getDataType() && getDataType()->getModuleKey()=="VuoColor";
		if (isColorPort)
		{
			double r=0, g=0, b=0, a=0;
			json_object *o = NULL;

			string colorRGBAJsonString = getConstantAsString();
			json_object *js = json_tokener_parse(colorRGBAJsonString.c_str());

			if (json_object_object_get_ex(js, "r", &o))
				r = json_object_get_double(o);
			if (json_object_object_get_ex(js, "g", &o))
				g = json_object_get_double(o);
			if (json_object_object_get_ex(js, "b", &o))
				b = json_object_get_double(o);
			if (json_object_object_get_ex(js, "a", &o))
				a = json_object_get_double(o);

			QColor constantColor = QColor(r*255, g*255, b*255, a*255);

			// Swatch border color
			constantFlagBorderColor = (constantColor.lightnessF()<=0.8? constantColor:untintedColors->nodeFill());
			constantFlagBorderColor.setAlphaF( fmax(0.2, constantFlagBorderColor.alphaF()) );  // make the border always visible

			// Swatch fill color
			constantFlagBackgroundBrush = constantColor;
		}

		if (isColorPort)
		{
			QPainterPath p = this->portConstantPath;

			// Provide an opaque background, so transparent colors can be distinguished from opaque colors.

			// getPortConstantPath() is assumed to give us points in this order:
			QPointF topRight = p.elementAt(0);
			QPointF rightTip = p.elementAt(1);
			QPointF bottomRight = p.elementAt(2);
			QPointF bottomLeftRoundedCornerBegin = p.elementAt(3);
			QPointF bottomLeftRoundedCornerEnd = p.elementAt(6);
			QPointF topLeftRoundedCornerBegin = p.elementAt(7);
			QPointF topLeftRoundedCornerEnd = p.elementAt(10);

			QPointF topLeftSharpEdge(topLeftRoundedCornerBegin.x(), topLeftRoundedCornerEnd.y());
			QPointF bottomLeftSharpEdge(bottomLeftRoundedCornerEnd.x(), bottomLeftRoundedCornerBegin.y());

			QPainterPath topHalf;
			topHalf.moveTo(bottomLeftSharpEdge);
			topHalf.lineTo(topLeftSharpEdge);
			topHalf.lineTo(topRight);
			topHalf.closeSubpath();
			painter->setClipPath(p);
			painter->fillPath(topHalf, Qt::black);

			QPainterPath bottomHalf;
			bottomHalf.moveTo(bottomLeftSharpEdge);
			bottomHalf.lineTo(topRight);
			bottomHalf.lineTo(rightTip);
			bottomHalf.lineTo(bottomRight);
			bottomHalf.closeSubpath();
			painter->setClipPath(p);
			painter->fillPath(bottomHalf, Qt::white);

			painter->setClipping(false);
		}

		// Constant flag
		painter->fillPath(this->portConstantPath, constantFlagBackgroundBrush);
		painter->fillPath(this->outsetMinusPortConstantPath, constantFlagBorderColor);

		// Constant string
		QRectF textRect = getPortConstantTextRectForText(constantText);
		painter->setPen(colors->portTitle());
		painter->setFont(VuoRendererFonts::getSharedFonts()->nodePortTitleFont());
		painter->drawText(textRect, Qt::AlignLeft, constantText);
	}

	paintEventBarrier(painter, colors);
	paintPortName(painter, colors);

	delete colors;
}

/**
 * Returns a boolean indicating whether this port has been deemed
 * eligible for selection based on its proximity to the cursor.
 */
bool VuoRendererPort::getEligibleForSelection()
{
	return isEligibleForSelection;
}

/**
 * Sets the boolean indicating whether this port is eligible
 * for direct connection to the cable currently being dragged between ports.
 */
void VuoRendererPort::setEligibleForDirectConnection(bool eligible)
{
	this->isEligibleForDirectConnection = eligible;
}

/**
 * Sets the boolean indicating whether this port is eligible
 * for typecast-assisted connection to the cable currently being dragged between ports.
 */
void VuoRendererPort::setEligibleForConnectionViaTypecast(bool eligible)
{
	this->isEligibleForConnectionViaTypecast = eligible;
}

/**
 * Handle mouse hover start events generated by custom code making use of an extended hover range.
 */
void VuoRendererPort::extendedHoverEnterEvent(bool cableDragUnderway)
{
	extendedHoverMoveEvent(cableDragUnderway);
}

/**
 * Handle mouse hover move events generated by custom code making use of an extended hover range.
 * If the optional @c highlightOnlyIfConnectable boolean is set to true, enable hover highlighting
 * for this port only if the port is eligible for connection to the cable currently being connected.
 */
void VuoRendererPort::extendedHoverMoveEvent(bool cableDragUnderway)
{
	QGraphicsItem::CacheMode normalCacheMode = cacheMode();
	setCacheMode(QGraphicsItem::NoCache);

	prepareGeometryChange();
	isEligibleForSelection = (cableDragUnderway? (isEligibleForDirectConnection || isEligibleForConnectionViaTypecast) : true);

	setCacheMode(normalCacheMode);

	setFocus();

	if (! cableDragUnderway)
	{
		vector<VuoCable *> connectedCables = getBase()->getConnectedCables(false);
		if (supportsDisconnectionByDragging() && (! connectedCables.empty()))
		{
			VuoRendererCable *cableToDisconnect = connectedCables.back()->getRenderer();
			cableToDisconnect->updateGeometry();
			cableToDisconnect->setHovered(true);
		}
	}
}

/**
 * Handle mouse hover leave events generated by custom code making use of an extended hover range.
 */
void VuoRendererPort::extendedHoverLeaveEvent()
{
	QGraphicsItem::CacheMode normalCacheMode = cacheMode();
	setCacheMode(QGraphicsItem::NoCache);

	prepareGeometryChange();
	isEligibleForSelection = false;

	setCacheMode(normalCacheMode);

	clearFocus();

	vector<VuoCable *> connectedCables = getBase()->getConnectedCables(false);
	if (supportsDisconnectionByDragging() && (! connectedCables.empty()))
	{
		VuoRendererCable *cableToDisconnect = connectedCables.back()->getRenderer();
		cableToDisconnect->updateGeometry();
		cableToDisconnect->setHovered(false);
	}
}

/**
 * Returns a boolean indicating whether there may be a cable
 * attached directly from this port to @c toPort, taking into account
 * the respective port types (input vs. output; event-only vs.
 * event+data; respective data types).
 *
 * If the connection would require one or both ports to be specialized, returns false.
 * (But see @c VuoRendererPort::canConnectDirectlyWithSpecializationTo(...).)
 */
bool VuoRendererPort::canConnectDirectlyWithoutSpecializationTo(VuoRendererPort *toPort)
{
	bool fromPortIsEnabledOutput = (this->getOutput() && this->isEnabled());
	bool toPortIsEnabledInput = (toPort->getInput() && toPort->isEnabled());
	
	if (fromPortIsEnabledOutput && toPortIsEnabledInput)
	{
		VuoType *fromDataType = this->getDataType();
		VuoType *toDataType = toPort->getDataType();

		// OK: Event-only to event+data.
		// OK: Event-only to event-only.
		if (!fromDataType)
			return true;

		// NOT OK (without a typeconverter): Event+data to event-only.
		else if (fromDataType && !toDataType)
			return false;

		// OK: Event+data to event+data, if types are non-generic and identical.
		else
			return (! dynamic_cast<VuoGenericType *>(fromDataType) && (fromDataType == toDataType));
	}
	return false;
}

/**
 * Returns a boolean indicating whether there may be a cable
 * attached directly from this port to @c toPort, taking into account
 * the respective port types (input vs. output; event-only vs.
 * event+data; respective data types), and the possibility that one
 * port may be specialized in preparation for the connection.
 *
 * Convenience function for VuoRendererPort::canConnectDirectlyWithSpecializationTo(const VuoRendererPort *toPort,
 * VuoRendererPort **portToSpecialize, string &specializedTypeName), for use
 * when only the returned boolean and none of the other output parameter values are needed.
 */
bool VuoRendererPort::canConnectDirectlyWithSpecializationTo(VuoRendererPort *toPort)
{
	VuoRendererPort *portToSpecialize = NULL;
	string specializedTypeName = "";

	return (this->canConnectDirectlyWithSpecializationTo(toPort, &portToSpecialize, specializedTypeName));
}

/**
 * Returns a boolean indicating whether there may be a cable
 * attached directly from this port to @c toPort, taking into account
 * the respective port types (input vs. output; event-only vs.
 * event+data; respective data types), and the possibility that one
 * port may be specialized in preparation for the connection.
 *
 * @param[out] toPort The port to consider connecting to.
 * @param[out] portToSpecialize The port, either this port or @c toPort, that will require specialization in order for the connection to be completed.
 *                              Does not account for potential cascade effects. May be NULL, if the connection may be completed without specialization.
 * @param[out] specializedTypeName The name of the specialized port type with which the generic port type is to be replaced.
 */
bool VuoRendererPort::canConnectDirectlyWithSpecializationTo(VuoRendererPort *toPort, VuoRendererPort **portToSpecialize, string &specializedTypeName)
{
	*portToSpecialize = NULL;
	specializedTypeName = "";

	if (this->canConnectDirectlyWithoutSpecializationTo(toPort))
		return true;

	bool fromPortIsEnabledOutput = (this->getOutput() && this->isEnabled());
	bool toPortIsEnabledInput = (toPort->getInput() && toPort->isEnabled());

	if (!(fromPortIsEnabledOutput && toPortIsEnabledInput))
		return false;

	VuoType *originalFromDataType = ((VuoCompilerPortClass *)(this->getBase()->getClass()->getCompiler()))->getDataVuoType();
	VuoType *originalToDataType = ((VuoCompilerPortClass *)(toPort->getBase()->getClass()->getCompiler()))->getDataVuoType();

	VuoType *currentFromDataType = this->getDataType();
	VuoType *currentToDataType = toPort->getDataType();

	if (!(originalFromDataType && originalToDataType && currentFromDataType && currentToDataType))
		return false;

	VuoGenericType *currentFromGenericType = dynamic_cast<VuoGenericType *>(currentFromDataType);
	VuoGenericType *currentToGenericType = dynamic_cast<VuoGenericType *>(currentToDataType);

	/// @todo (https://b33p.net/kosada/node/7032)
	if (VuoType::isListTypeName(currentFromDataType->getModuleKey()) != VuoType::isListTypeName(currentToDataType->getModuleKey()))
		return false;

	// Case: The 'From' and 'To' ports are both generics.
	if (currentFromGenericType && currentToGenericType)
		return (currentFromGenericType->isGenericTypeCompatible(currentToGenericType));

	// Case: The 'From' port is generic and can be specialized to match the concrete type of the 'To' port.
	if (currentFromGenericType && currentFromGenericType->isSpecializedTypeCompatible(originalToDataType->getModuleKey()))
	{
		*portToSpecialize = this;
		specializedTypeName = originalToDataType->getModuleKey();

		return true;
	}

	// Case: The 'To' port is generic and can be specialized to match the concrete type of the 'From' port.
	else if (currentToGenericType && currentToGenericType->isSpecializedTypeCompatible(originalFromDataType->getModuleKey()))
	{
		*portToSpecialize = toPort;
		specializedTypeName = originalFromDataType->getModuleKey();

		return true;
	}

	return false;
}

/**
 * Returns a boolean indicating whether there is a cable
 * connecting this port to @c toPort.
 */
bool VuoRendererPort::isConnectedTo(VuoRendererPort *toPort)
{
	vector<VuoCable *> cables = this->getBase()->getConnectedCables(false);
	for (vector<VuoCable *>::iterator cable = cables.begin(); cable != cables.end(); ++cable)
		if ((*cable)->getToPort() == toPort->getBase())
			return true;

	return false;
}

/**
 * Handle mouse double-click events.
 */
void VuoRendererPort::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if (isConstant())
	{
		signaler->signalInputEditorRequested(this);
	}
}

/**
 * Handle key-press events.
 */
void VuoRendererPort::keyPressEvent(QKeyEvent *event)
{
	if (isConstant() && event->key() == Qt::Key_Return)
	{
		signaler->signalInputEditorRequested(this);
	}
}

/**
 * Returns a boolean indicating whether this port supports cable disconnection
 * by dragging from the port.
 */
bool VuoRendererPort::supportsDisconnectionByDragging(void)
{
	return (getInput() &&
			(! isConstant()) &&
			(! dynamic_cast<VuoRendererTypecastPort *>(this)) &&
			(! getAttachedInputDrawer()));
}

/**
 * Returns a boolean indicating whether this port is an input port.
 */
bool VuoRendererPort::getInput(void) const
{
	return ((! isOutput) && (! isFunctionPort));
}

/**
 * Returns a boolean indicating whether this port is an output port.
 */
bool VuoRendererPort::getOutput(void) const
{
	return isOutput;
}

/**
 * Returns true if this port is a refresh port.
 */
bool VuoRendererPort::getRefreshPort(void) const
{
	return isRefreshPort;
}

/**
 * Returns true if this port is a done port.
 */
bool VuoRendererPort::getDonePort(void) const
{
	return isDonePort;
}

/**
 * Returns true if this port is a function port.
 */
bool VuoRendererPort::getFunctionPort(void) const
{
	return isFunctionPort;
}

/**
 * Schedules a redraw of this port.
 */
void VuoRendererPort::updateGeometry()
{
	this->prepareGeometryChange();
}

/**
 * Updates the port to reflect changes in state.
 */
QVariant VuoRendererPort::itemChange(GraphicsItemChange change, const QVariant &value)
{
	// Port has moved relative to its parent
	if (change == QGraphicsItem::ItemPositionHasChanged)
	{
		VuoRendererNode *parentNode = getRenderedParentNode();
		if (parentNode)
			parentNode->layoutConnectedInputDrawersAtAndAbovePort(this);
	}

	return QGraphicsItem::itemChange(change, value);
}

/**
 * Returns the data type associated with this port,
 * or NULL if there is no associated data type.
 */
VuoType * VuoRendererPort::getDataType(void) const
{
	if (!getBase()->hasCompiler())
		return NULL;

	VuoCompilerPort *compilerPort = static_cast<VuoCompilerPort *>(getBase()->getCompiler());
	return compilerPort->getDataVuoType();
}

/**
 * Returns true if this port has a constant data value.
 */
bool VuoRendererPort::isConstant(void) const
{
	return ((getInput() && getDataType()) &&						// input port with data...
			(! ((VuoCompilerPort *)(getBase()->getCompiler()))->hasConnectedDataCable(true)));	// ... that has no incoming data cable (published or unpublished).
}

/**
 * Returns the string representation of this port's constant data value, or an empty string if it has none.
 */
string VuoRendererPort::getConstantAsString(void) const
{
	if (!(getInput() && getDataType()))
		return "";

	VuoCompilerInputEventPort *compilerPort = static_cast<VuoCompilerInputEventPort *>(getBase()->getCompiler());
	return compilerPort->getData()->getInitialValue();
}

/**
 * Returns the string representation of this port's constant data value as it should be rendered
 * in its constant data flag, or an empty string if it has no currently assigned constant data value.
 */
string VuoRendererPort::getConstantAsStringToRender(void) const
{
	if (!(getInput() && getDataType()))
		return "";

	/// @todo Implement with input viewers (https://b33p.net/kosada/node/5700)
	if (getDataType())
	{
		// Don't display constant input values for generic ports.
		if (dynamic_cast<VuoGenericType *>(getDataType()))
			return "";

		if (getDataType()->getModuleKey()=="VuoColor")
		{
			return "   ";
		}
		if (getDataType()->getModuleKey()=="VuoText")
		{
			json_object *js = json_tokener_parse(getConstantAsString().c_str());
			string textWithoutQuotes;
			if (json_object_get_type(js) == json_type_string)
				textWithoutQuotes = json_object_get_string(js);
			json_object_put(js);

			// Abbreviate long VuoText data with an ellipsis.
			if (textWithoutQuotes.length() > 30)
				textWithoutQuotes = textWithoutQuotes.substr(0, 27) + "...";

			return textWithoutQuotes;
		}
		if (getDataType()->getModuleKey()=="VuoReal")
		{
			json_object *js = json_tokener_parse(getConstantAsString().c_str());
			double real = json_object_get_double(js);
			json_object_put(js);

			QString valueAsStringInUserLocale = QLocale::system().toString(real);
			if (qAbs(real) >= 1000.0)
				valueAsStringInUserLocale.remove(QLocale::system().groupSeparator());

			return valueAsStringInUserLocale.toStdString();
		}

		if (getDataType()->getModuleKey()=="VuoPoint2d")
		{
			json_object *js = json_tokener_parse(getConstantAsString().c_str());
			double x = 0, y = 0;
			json_object *o = NULL;
			if (json_object_object_get_ex(js, "x", &o))
				x = json_object_get_double(o);
			if (json_object_object_get_ex(js, "y", &o))
				y = json_object_get_double(o);
			json_object_put(js);

			QList<double> pointList = QList<double>() << x << y;
			return getPointStringForCoords(pointList);
		}
		if (getDataType()->getModuleKey()=="VuoPoint3d")
		{
			json_object *js = json_tokener_parse(getConstantAsString().c_str());
			double x = 0, y = 0, z = 0;
			json_object *o = NULL;
			if (json_object_object_get_ex(js, "x", &o))
				x = json_object_get_double(o);
			if (json_object_object_get_ex(js, "y", &o))
				y = json_object_get_double(o);
			if (json_object_object_get_ex(js, "z", &o))
				z = json_object_get_double(o);
			json_object_put(js);

			QList<double> pointList = QList<double>() << x << y << z;
			return getPointStringForCoords(pointList);
		}
		if (getDataType()->getModuleKey()=="VuoPoint4d")
		{
			json_object *js = json_tokener_parse(getConstantAsString().c_str());
			double x = 0, y = 0, z = 0, w = 0;
			json_object *o = NULL;
			if (json_object_object_get_ex(js, "x", &o))
				x = json_object_get_double(o);
			if (json_object_object_get_ex(js, "y", &o))
				y = json_object_get_double(o);
			if (json_object_object_get_ex(js, "z", &o))
				z = json_object_get_double(o);
			if (json_object_object_get_ex(js, "w", &o))
				w = json_object_get_double(o);
			json_object_put(js);

			QList<double> pointList = QList<double>() << x << y << z << w;
			return getPointStringForCoords(pointList);
		}
		if (getDataType()->getModuleKey()=="VuoFont")
		{
			json_object *js = json_tokener_parse(getConstantAsString().c_str());
			json_object *o = NULL;

			const char *fontName = NULL;
			if (json_object_object_get_ex(js, "fontName", &o))
				fontName = json_object_get_string(o);

			double pointSize = 0;
			if (json_object_object_get_ex(js, "pointSize", &o))
				pointSize = json_object_get_double(o);

			bool underline = false;
			if (json_object_object_get_ex(js, "underline", &o))
				underline = json_object_get_boolean(o);
			const char *underlineString = underline ? " [U]" : "";

			const char *outputString = "";
			if (fontName)
				outputString = QString("%1 %2pt%3").arg(fontName).arg(pointSize).arg(underlineString).toUtf8().data();

			json_object_put(js);

			return outputString;
		}
	}

	return getConstantAsString();
}

/**
 * Sets this port's constant data value to that represented by the provided @c constantValue string.
 */
void VuoRendererPort::setConstant(string constantValue)
{
	QGraphicsItem::CacheMode normalCacheMode = cacheMode();
	setCacheMode(QGraphicsItem::NoCache);

	updateGeometry();
	VuoCompilerInputEventPort *eventPort = static_cast<VuoCompilerInputEventPort *>(getBase()->getCompiler());
	eventPort->getData()->setInitialValue(constantValue);

	updatePortConstantPath();

	setCacheMode(normalCacheMode);

	getRenderedParentNode()->layoutConnectedInputDrawersAtAndAbovePort(eventPort->getBase()->getRenderer());
}

/**
  * Returns a boolean indicating whether the port name should be rendered along with this port,
  * taking into account the port's own attributes as well as whether the port will be
  * rendered within a published port sidebar.
  */
bool VuoRendererPort::portNameRenderingEnabled() const
{
	bool sidebarPaintMode = getProxyPublishedSidebarPort();
	string name = (sidebarPaintMode? getProxyPublishedSidebarPort()->getBase()->getName() : getBase()->getClass()->getName());

	if (name.empty() || isAnimated)
		return false;
	else if (sidebarPaintMode)
		return true;
	else if (isRefreshPort || isDonePort || isFunctionPort || typecastParentPort)
		return false;

	return true;
}

/**
  * Given a list of coordinates, returns the string representation of the point
  * consisting of those coordinate values as it should be rendered within a
  * constant data flag.
  *
  * Helper function for @c VuoRendererPort::getConstantAsStringToRender().
  */
string VuoRendererPort::getPointStringForCoords(QList<double> coordList) const
{
	const QString coordSeparator = QString(QLocale::system().decimalPoint() != ','? QChar(',') : QChar(';')).append(" ");
	QStringList coordStringList;

	foreach (double coord, coordList)
	{
		QString valueAsStringInUserLocale = QLocale::system().toString(coord);
		if (qAbs(coord) >= 1000.0)
			valueAsStringInUserLocale.remove(QLocale::system().groupSeparator());

		coordStringList.append(valueAsStringInUserLocale);
	}

	QString pointString = QString("(").append(coordStringList.join(coordSeparator).append(")"));
	return pointString.toStdString();
}

/**
 * Returns a boolean indicating whether this port is publishable.
 */
bool VuoRendererPort::getPublishable() const
{
	// @todo: Allow generic published ports (https://b33p.net/kosada/node/7655).
	bool isGeneric = bool(dynamic_cast<VuoGenericType *>(this->getDataType()));

	// @todo: Allow direct connections between external published inputs and external published outputs
	// (https://b33p.net/kosada/node/7756).
	bool isExternalPublishedPort = getProxyPublishedSidebarPort();

	return (!isGeneric && !isExternalPublishedPort);
}

/**
 * Returns a vector of pointers to the externally visible published ports
 * connected to this port.
 */
vector<VuoRendererPublishedPort *> VuoRendererPort::getPublishedPorts(void) const
{
	vector <VuoRendererPublishedPort *> publishedPorts;
	foreach (VuoCable *cable, getBase()->getConnectedCables(true))
	{
		if (getInput() && cable->isPublishedInputCable())
			publishedPorts.push_back(cable->getFromPort()->getRenderer()->getProxyPublishedSidebarPort());
		else if (getOutput() && cable->isPublishedOutputCable())
			publishedPorts.push_back(cable->getToPort()->getRenderer()->getProxyPublishedSidebarPort());
	}

	return publishedPorts;
}

/**
 * If set, this port will not be drawn; its drawing will be handled by @c proxySidebarPort.  Used for ports
 * belonging to "vuo.in" or "vuo.out" nodes, to be rendered within the published port sidebars
 * rather than on the canvas.
 */
void VuoRendererPort::setProxyPublishedSidebarPort(VuoRendererPublishedPort *proxySidebarPort)
{
	this->proxyPublishedSidebarPort = proxySidebarPort;
	updateEnabledStatus();
}

/**
 * Returns this port's published port sidebar renderering proxy, or NULL if it has none.
 */
VuoRendererPublishedPort * VuoRendererPort::getProxyPublishedSidebarPort(void) const
{
	return proxyPublishedSidebarPort;
}

/**
 * Resets the time that the last event was fired to a value that causes
 * the port to be painted as if activity-rendering were disabled.
 */
void VuoRendererPort::resetTimeLastEventFired()
{
	this->timeLastEventFired = VuoRendererColors::getVirtualFiredEventOrigin();
}

/**
 * Updates the port's state to indicate that it has just fired an event.
 */
void VuoRendererPort::setFiredEvent()
{
	this->timeLastEventFired = QDateTime::currentMSecsSinceEpoch();
}

/**
 * Updates the port's state to indicate that it fired an event at such a time
 * that its fade percentage should now be equal to @c percentage.
 */
void VuoRendererPort::setFadePercentageSinceEventFired(qreal percentage)
{
	this->timeLastEventFired = VuoRendererColors::getVirtualFiredEventOriginForAnimationFadePercentage(percentage);
}

/**
 * Returns the 'Show Events'-mode animations associated with this port.
 */
vector<QGraphicsItemAnimation *> VuoRendererPort::getAnimations()
{
	return this->animations;
}

/**
 * Sets the boolean indicating whether this port is an animation, and not
 * itself a component of the base composition.
 */
void VuoRendererPort::setAnimated(bool animated)
{
	this->isAnimated = animated;
	updateEnabledStatus();
}

/**
 * Sets the cache mode of this port, and any child ports, to @c mode.
 */
void VuoRendererPort::setCacheModeForPortAndChildren(QGraphicsItem::CacheMode mode)
{
	this->setCacheMode(mode);

	VuoRendererTypecastPort *typecastPort = dynamic_cast<VuoRendererTypecastPort *>(this);
	if (typecastPort)
		typecastPort->getChildPort()->setCacheMode(mode);
}

/**
 * Determines whether this port will accept mouse events based on
 * the port's current attributes.
 */
void VuoRendererPort::updateEnabledStatus()
{
	// Port animations, and ports without compilers, shouldn't accept mouse events.
	setEnabled(!isAnimated && 
			   ((getBase()->hasCompiler() && getBase()->getClass()->hasCompiler()) ||
				getProxyPublishedSidebarPort()));
}
