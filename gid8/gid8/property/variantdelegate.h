#ifndef VARIANTDELEGATE_H
#define VARIANTDELEGATE_H

#include <QStyledItemDelegate>
#include <QRegularExpression>
#include <QSharedPointer>

struct TypeChecker
{
    TypeChecker();

    QRegularExpression boolExp;
    QRegularExpression byteArrayExp;
    QRegularExpression charExp;
    QRegularExpression colorExp;
    QRegularExpression dateExp;
    QRegularExpression dateTimeExp;
    QRegularExpression doubleExp;
    QRegularExpression pointExp;
    QRegularExpression rectExp;
    QRegularExpression signedIntegerExp;
    QRegularExpression sizeExp;
    QRegularExpression timeExp;
    QRegularExpression unsignedIntegerExp;
};

class VariantDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit VariantDelegate(const QSharedPointer<TypeChecker> &typeChecker,
                             QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    static QLineEdit *createLineEditor(QWidget *parent, TypeChecker *m_typeChecker,
//        const QStyleOptionViewItem & /* option */,
        const QVariant originalValue, QMetaType type);
    
    
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    static bool isSupportedType(int type);
    static QString displayText(const QVariant &value);

    void setValidator(QLineEdit * lineEdit, QVariant originalValue);
private:
    QSharedPointer<TypeChecker> m_typeChecker;


private slots:
    void onActionTriggered();


};

#endif
