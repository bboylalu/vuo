/**
 * @file
 * VuoRendererPort interface.
 *
 * @copyright Copyright © 2012–2014 Kosada Incorporated.
 * This interface description may be modified and distributed under the terms of the GNU Lesser General Public License (LGPL) version 2 or later.
 * For more information, see http://vuo.org/license.
 */

#ifndef VUORENDERERPORT_HH
#define VUORENDERERPORT_HH

#include "VuoRendererItem.hh"

#include "VuoBaseDetail.hh"
#include "VuoType.hh"
#include "VuoPortClass.hh"

class VuoCompilerNodeClass;
class VuoCompilerNode;
class VuoRendererNode;
class VuoRendererMakeListNode;
class VuoCable;
class VuoPort;
class VuoRendererColors;
class VuoRendererPublishedPort;
class VuoRendererSignaler;

/**
 * Renders a node's port in a @c QGraphicsScene.  Typically automatically created by a @c VuoRendererNode instance.
 */
class VuoRendererPort : public VuoRendererItem, public VuoBaseDetail<VuoPort>
{
public:
	VuoRendererPort(VuoPort *basePort, VuoRendererSignaler *signaler,
					bool isOutput, bool isRefreshPort, bool isDonePort, bool isFunctionPort);

	QRectF boundingRect(void) const;
	QPainterPath shape(void) const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	VuoRendererPublishedPort * getProxyPublishedSidebarPort(void) const;
	void setProxyPublishedSidebarPort(VuoRendererPublishedPort *proxyPort);

	bool getEligibleForSelection();
	void setEligibleForDirectConnection(bool eligible);
	void setEligibleForConnectionViaTypecast(bool eligible);
	void extendedHoverEnterEvent(bool cableDragUnderway=false);
	void extendedHoverMoveEvent(bool cableDragUnderway=false);
	void extendedHoverLeaveEvent();
	bool canConnectDirectlyWithoutSpecializationTo(VuoRendererPort *toPort);
	bool canConnectDirectlyWithSpecializationTo(VuoRendererPort *toPort);
	bool canConnectDirectlyWithSpecializationTo(VuoRendererPort *toPort, VuoRendererPort **portToSpecialize, string &specializedTypeName);
	bool isConnectedTo(VuoRendererPort *toPort);
	bool getInput(void) const;
	bool getOutput(void) const;
	bool getRefreshPort(void) const;
	bool getDonePort(void) const;
	bool getFunctionPort(void) const;
	void updateGeometry();
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	QPainterPath getPortPath(qreal inset) const;
	static QPainterPath getPortConstantPath(QRectF innerPortRect, QString text, QPainterPath *outsetPath, bool isTypecast=false);
	void updatePortConstantPath();
	static QRectF getPortRect(void);
	VuoRendererMakeListNode * getAttachedInputDrawer(void) const;
	virtual QRectF getPortConstantTextRect(void) const;
	VuoType * getDataType(void) const;
	bool isConstant(void) const;
	string getConstantAsString(void) const;
	string getConstantAsStringToRender(void) const;
	void setConstant(string constantValue);
	bool getPublishable() const;
	vector<VuoRendererPublishedPort *> getPublishedPorts() const;

	VuoRendererNode * getUnderlyingParentNode(void) const;
	VuoRendererNode * getRenderedParentNode(void) const;

	VuoRendererPort * getTypecastParentPort() const;
	void setTypecastParentPort(VuoRendererPort *typecastParentPort);
	bool supportsDisconnectionByDragging(void);

	void resetTimeLastEventFired();
	void setFiredEvent();
	void setFadePercentageSinceEventFired(qreal percentage);
	void setCacheModeForPortAndChildren(QGraphicsItem::CacheMode mode);

	vector<QGraphicsItemAnimation *> getAnimations();
	void setAnimated(bool animated);

	// Drawing configuration
	static const qreal portRadius; ///< Radius, in pixels at 1:1 zoom, of a circular port.
	static const qreal portSpacing; ///< Vertical distance, in pixels at 1:1 zoom, between the center points of two ports.
	static const qreal portContainerMargin;	///< Vertical distance, in pixels at 1:1 zoom, between the outer edge of the first/last port and the node frame rect.
	static const qreal portInset; ///< The vertical and horizontal inset used when rendering a port shape within its outer port rect.
	static const qreal constantFlagHeight; ///< Height, in pixels at 1:1 zoom, of a constant flag.

protected:
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);

private:

	// Port attributes affecting drawing
	bool isOutput;
	bool isFunctionPort;
	bool isEligibleForDirectConnection;
	bool isEligibleForConnectionViaTypecast;
	bool isEligibleForSelection;
	bool isAnimated;
	VuoRendererPort *typecastParentPort;
	VuoRendererPublishedPort *proxyPublishedSidebarPort;
	qint64 timeLastEventFired;
	vector<QGraphicsItemAnimation *> animations;

	QRectF portHybridRect;
	QPainterPath portConstantPath;
	QPainterPath outsetPath;
	QPainterPath outsetMinusPortConstantPath;
	
	friend class VuoRendererPublishedPort; 	///< VuoRendererPublishedPort needs paint(...) and boundingRect()
	friend class TestVuoRenderer;

protected:
	bool isRefreshPort; ///< Is this port a refresh port?
	bool isDonePort; ///< Is this port a done port?

	QRectF nameRect; ///< The bounding box of the port's label when rendered on the canvas.

	bool portNameRenderingEnabled(void) const;
	QRectF getNameRect(void) const;
	void updateNameRect(void);
	bool isOnDrawer(void) const;
	void updateEnabledStatus();

	static QRectF getPortConstantTextRectForText(QString text);
	static QPainterPath getPortPath(qreal inset, VuoPortClass::PortType portType, bool isInputPort, bool carriesData);
	QPainterPath getFunctionPortGlyph(void) const;

	void paintPortName(QPainter *painter, VuoRendererColors *colors);
	void paintEventBarrier(QPainter *painter, VuoRendererColors *colors);
	string getPointStringForCoords(QList<double>) const;

	VuoRendererSignaler *signaler; ///< The Qt signaler used by this port.
};

#endif // VUORENDERERPORT_HH
