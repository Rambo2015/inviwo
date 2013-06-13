#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneEvent>
#include <inviwo/qt/editor/transferfunctioneditorcontrolpoint.h>

namespace inviwo {

    TransferFunctionEditorControlPoint::TransferFunctionEditorControlPoint(TransferFunctionDataPoint* datapoint):datapoint_(datapoint){
		setFlag(QGraphicsItem::ItemIsMovable);
		setFlag(QGraphicsItem::ItemIsSelectable);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges);
        setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
		setFlag(QGraphicsItem::ItemIgnoresTransformations, false);

        setPos(datapoint_->getPos()->x, datapoint_->getPos()->y);
        setZValue(1);
		size_ = 12.0f;
    }

    TransferFunctionDataPoint* TransferFunctionEditorControlPoint::getPoint(){
        return datapoint_;
    }

    void TransferFunctionEditorControlPoint::setId(int id){
        id_ = id;
    }

    const int TransferFunctionEditorControlPoint::getId(){
        return id_;
    }

    TransferFunctionEditorControlPoint::TransferFunctionEditorControlPoint(){};

    TransferFunctionEditorControlPoint::~TransferFunctionEditorControlPoint(){
		delete datapoint_;
	};

    void TransferFunctionEditorControlPoint::paint(QPainter* painter, const QStyleOptionGraphicsItem* options, QWidget* widget) {
        IVW_UNUSED_PARAM(options);
        IVW_UNUSED_PARAM(widget);
        painter->setRenderHint(QPainter::Antialiasing, true);
        QPen* pen;
		QBrush* brush;

		brush = new QBrush(QColor::fromRgbF(datapoint_->getRgba()->r, datapoint_->getRgba()->g, datapoint_->getRgba()->b));
		
        if (this->isSelected()){
            pen = new QPen(Qt::red, 2.5, Qt::SolidLine, Qt::RoundCap);
        } 
        else{
            pen = new QPen(Qt::black, 2.5, Qt::SolidLine, Qt::RoundCap);
        }
        painter->setPen(*pen);
		painter->setBrush(*brush);
		painter->drawEllipse(-size_/2, -size_/2, size_, size_);

		delete pen;
		delete brush;
    }

    QRectF TransferFunctionEditorControlPoint::boundingRect() const {
		float bboxsize = size_ + 10.0f;
        float left = pos().x() - bboxsize/2;
        float top = pos().y() - bboxsize/2;
        return QRectF(-bboxsize/2, -bboxsize/2, bboxsize, bboxsize);
    }

    void TransferFunctionEditorControlPoint::mousePressEvent ( QGraphicsSceneMouseEvent *e ){
        this->setSelected(true);
    }

    void TransferFunctionEditorControlPoint::mouseReleaseEvent( QGraphicsSceneMouseEvent *e ){}

	void TransferFunctionEditorControlPoint::mouseMoveEvent(QGraphicsSceneMouseEvent * e)
	{
		vec2 pos = vec2(e->scenePos().x(), e->scenePos().y());

		if (pos.x <= 0.0f){
			pos.x = 0.0f;
		} 
		if (pos.x >= 255.0f){
			pos.x = 255.0f;
		}
		if (pos.y <= 0.0f){
			pos.y = 0.0f;
		} 
		if (pos.y >= 100.0f){
			pos.y = 100.0f;
		}

		pos.x = floor(pos.x + 0.5);

		this->setPos(QPointF(pos.x, pos.y));
		this->datapoint_->setPos(pos);
		this->datapoint_->setA(pos.y/100.0f);
	}
} // namespace