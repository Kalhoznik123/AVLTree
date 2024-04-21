#include <iostream>
#include <stack>
#include <cassert>
#include <cmath>
#include <iterator>
#include <algorithm>
#include <memory>

using namespace std;


//TODO:: добавить итреаторы в insert и remove
//TODO:: добавить поддержку аллокаторов
//TODO:: добавить конструктор с листом инициализации
//TODO:: добавить операторы сравнения
//TODO:: конструкторы

template <typename Type,
          typename Compare = std::less<Type>,
          typename Allocator = std::allocator<Type>>
class AVLTree{

private:
    class Node {
    public:
        Node( Type k, Node* l=nullptr, Node* r=nullptr,Node* parent=nullptr, size_t height = 1) :
            value_(k), left_(l), right_(r),
            parent_(parent),height_(height) {}
        Type value_;
        Node* left_;
        Node* right_;
        Node* parent_;
        size_t height_;
    };

    template<typename ValueType>
    class BasicIterator{
    public:

        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator (const BasicIterator& other) = default;
        BasicIterator& operator= (const BasicIterator& other) = default;

        bool operator!=(const BasicIterator<const Type>& rhs) const noexcept{

            return  node_ != rhs.node_;
        }
        bool operator!=(const BasicIterator<Type>& rhs) const noexcept{
            return  node_ != rhs.node_;
        }

        bool operator==(const BasicIterator<const Type>& rhs) const noexcept{
            return  !(node_ != rhs.node_);
        }
        bool operator==(const BasicIterator<Type>& rhs) const noexcept{
            return  !(node_ != rhs.node_);
        }

        reference operator*() const noexcept{
            return node_->value_;
        }

        pointer operator->() const noexcept{
            return &node_->value_;
        }
        BasicIterator& operator++() noexcept{
            prev_node_ = node_;
            node_ = NextNode(node_);
            return *this;
        }
        BasicIterator operator++(int) noexcept {
            BasicIterator temp(node_,prev_node_);
            prev_node_ = node_;
            node_ = NextNode(node_);
            return temp;
        }

        BasicIterator& operator--() noexcept{
            node_ = prev_node_;
            prev_node_ = PrevNode(node_);
            return *this;
        }
        BasicIterator operator--(int) noexcept {
            BasicIterator temp(node_,prev_node_);
            node_ = prev_node_;
            prev_node_ = PrevNode(node_);
            return temp;
        }

    private:

        Node* NextNode(Node* vertex){
            if(vertex->right_ != nullptr){
                return TreeMin(vertex->right_);
            }
            Node* result = vertex->parent_;
            while(result != nullptr && vertex == result->right_){
                vertex = result;
                result = result->parent_;
            }
            return  result;
        }
        Node* PrevNode(Node* vertex){
            if(vertex->left_ != nullptr){
                return TreeMax(vertex->left_);
            }
            Node* result = vertex->parent_;
            while(result != nullptr && vertex == result->left_){
                vertex = result;
                result = result->parent_;
            }
            return  result;
        }


    private:
        friend class AVLTree;
        explicit BasicIterator(Node* node,Node* prev_node_):node_(node),
            prev_node_(prev_node_){}
        Node* node_ = nullptr;
        Node* prev_node_ = nullptr;
    };


public:

    using value_type = Type;
    using reference = Type&;
    using const_reference = const Type&;
    using difference_type =  std::ptrdiff_t;
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;
    using Reverse_iterator = std::reverse_iterator<Iterator>;
    using Const_reverse_iterator = std::reverse_iterator<ConstIterator>;

    AVLTree():comp_(Compare()) {};

    AVLTree(const Compare& comp):comp_(comp){};

    AVLTree(const AVLTree& rhs){
        root_ = CopyHelper(rhs.root_,root_);
        FillParentPointers(root_);
        size_ = rhs.size_;
        comp_ = rhs.comp_;
    }

    AVLTree& operator= (const AVLTree& rhs){
        if(this == &rhs){
            return *this;
        }
        AVLTree temp(rhs);
        Swap(temp);
        return *this;
    }

    ~AVLTree(){
        AVLClear(root_);
    }

    //сравнение это не поэлементное счравнение а все ли элементыодного множества содержатсяв другом
    bool operator==(const AVLTree& rhs) const{
        return  (size_ == rhs.size_) && std::equal(begin(),end(),rhs.begin(),rhs.end(),std::equal_to{});
    }
    bool operator!=(const AVLTree& rhs) const{
        return  !(*this == rhs);
    }

    void Swap(AVLTree& rhs){
        std::swap(size_,rhs.size_);
        std::swap(root_,rhs.root_);
        std::swap(comp_,rhs.comp_);
    }

    void Insert(const Type& key){
        root_ = InsertHelper(root_,key);
    }

    size_t Size() const{
        return  size_;
    }
    bool Empty(){
        return  size_ == 0;
    }
    void Remove(const Type& val_to_del){
        if(!Empty())
            root_ = RemoveNode(root_,val_to_del);
    }
    value_type Find(const Type& value_to_find){
        Node* res = FindHelper(root_,value_to_find);
        return  res != nullptr ? res->value_ : value_type();
    }
    [[nodiscard]] Iterator begin() noexcept {
        Node* node = TreeMin(root_);
        Node* prev_node = nullptr;
        return Iterator{node,prev_node};
    }
    [[nodiscard]] Iterator end() noexcept {
        Node* prev_node = TreeMax(root_);
        return Iterator{prev_node->right_, prev_node};
    }
    // Константные версии begin/end для обхода списка без возможности модификации его элементов
    [[nodiscard]] ConstIterator begin() const noexcept {
        Node* node = TreeMin(root_);
        Node* prev_node = nullptr;
        return ConstIterator{node,prev_node};
    }
    [[nodiscard]] ConstIterator end() const noexcept {
        Node* prev_node = TreeMax(root_);
        return ConstIterator{prev_node->right_, prev_node};
    }
    // Методы для удобного получения константных итераторов у неконстантного контейнера
    [[nodiscard]] ConstIterator cbegin() const noexcept {
        Node* node = TreeMin(root_);
        Node* prev_node = nullptr;
        return ConstIterator{node,prev_node};
    }
    [[nodiscard]] ConstIterator cend() const noexcept {
        Node* prev_node = TreeMax(root_);
        return ConstIterator{prev_node->right_, prev_node};
    }
    [[nodiscard]] Reverse_iterator rbegin()  noexcept {
        return Reverse_iterator(end());
    }
    [[nodiscard]] Reverse_iterator rend()  noexcept {
        return Reverse_iterator(begin());
    }

    [[nodiscard]] Const_reverse_iterator rbegin() const noexcept {
        return Const_reverse_iterator(end());
    }
    [[nodiscard]] Const_reverse_iterator rend() const noexcept {
        return Const_reverse_iterator(begin());
    }
    [[nodiscard]] Const_reverse_iterator crbegin() const noexcept {
        return Const_reverse_iterator(cend());
    }
    [[nodiscard]] Const_reverse_iterator crend() const noexcept {
        return Const_reverse_iterator(cbegin());
    }


private:
    using AllocTraits = std::allocator_traits<Allocator>;

    Node* root_ = nullptr;
    size_t size_{0};    
    Compare comp_;
    typename AllocTraits::template rebind_alloc<Node> alloc;
    using AllocTraitsNode = typename AllocTraits::template rebind_traits<Node>;

    size_t Height(Node* p){
        return p?p->height_:0;
    }

    void FixHeight(Node* p){
        size_t hl = Height(p->left_);
        size_t hr = Height(p->right_);
        p->height_ = (hl>hr?hl:hr)+1;
    }

    void AVLClear(Node* root){
        if(root != nullptr){
            AVLClear(root->left_);
            AVLClear(root->right_);

            root->~Node();
            AllocTraitsNode::deallocate(alloc,root,1);
            //delete  root;
        }
    }

    Node* RemoveNode(Node* root, Type val_to_del);



    Node* FindHelper(Node* root, const Type& value_to_find) const;
    Node* CopyHelper(const Node* lhs, Node* root);
    void FillParentPointers(Node* root);
    Node* SmallLeftRotation(Node* a);
    Node* SmallRightRotation(Node* a);
    size_t GetHeight(Node* vertex);
    Node* BigLeftRotation(Node* v);
    Node* BigRightRotation(Node* v);
    Node* Rotate(Node* vertex);
    Node* InsertHelper(Node* vertex, const Type& value);
    static  Node* TreeMin(Node* root);
    static  Node* TreeMax(Node* root);

};

template<typename Type, typename Compare, typename Allocator>
typename AVLTree<Type,Compare,Allocator>::Node* AVLTree<Type, Compare, Allocator>::RemoveNode(Node *root, Type val_to_del){
    if (root == nullptr){
        return nullptr;
    }
    else{
        // нода найдена  и ее нужно удалить
        if(val_to_del == root->value_){
            if (root->left_ == nullptr && root->right_ == nullptr){
                root->~Node();
                AllocTraitsNode::deallocate(alloc,root,1);
                --size_;
                return nullptr;
            }else if(root->left_ == nullptr){ // у ноды только правый ребенок
                Node* temp = root->right_;
                root->~Node();
                AllocTraitsNode::deallocate(alloc,root,1);
                --size_;
                return temp;
            }else if (root->right_ == nullptr){ // у ноды только левый  ребенок
                Node* temp = root->left_;
                root->~Node();
                AllocTraitsNode::deallocate(alloc,root,1);
                --size_;
                return temp;
            }else{
                // находим максимальную ноду в левом поддереве
                Node* min_sub = root->left_;
                while (min_sub->right_ != nullptr){
                    min_sub = min_sub->right_;
                }
                root->value_ = min_sub->value_;// заменяем удаляемое значение на самую большую левую ноду
                root->left_ = RemoveNode(root->left_, min_sub->value_);// запускаем алгоритм для удаления не нужной максимальной левой ноды
                root = Rotate(root);
            }
        }else{
            if (comp_(val_to_del,root->value_)){
                root->left_ = RemoveNode(root->left_, val_to_del);
            }
            else if (!comp_(val_to_del,root->value_)){
                root->right_ = RemoveNode(root->right_, val_to_del);
            }
        }
        return Rotate(root);
    }
}

template<typename Type, typename Compare, typename Allocator>
typename AVLTree<Type,Compare,Allocator>::Node* AVLTree<Type, Compare, Allocator>::FindHelper(Node *root, const Type &value_to_find) const {

    if(root == nullptr){
        return  nullptr;
    }else{
        if(root->value_ == value_to_find){
            return root;
        }else if(comp_(root->value_,value_to_find)){
            return FindHelper(root->right_,value_to_find);
        }else{
            return FindHelper(root->left_,value_to_find);
        }
    }
}

template<typename Type, typename Compare, typename Allocator>
typename AVLTree<Type,Compare,Allocator>::Node* AVLTree<Type, Compare, Allocator>::CopyHelper(const Node *lhs, Node *root){

    if(root == nullptr && lhs != nullptr){

        root = AllocTraitsNode::allocate(alloc,1);
        AllocTraitsNode::construct(alloc,root,lhs->value_);
        //root = new Node(lhs->value_);
    }else{
        return nullptr;
    }
    root->left_  = CopyHelper(lhs->left_,root->left_);
    root->right_ = CopyHelper(lhs->right_,root->right_);
    return root;
}

template<typename Type, typename Compare, typename Allocator>
void AVLTree<Type, Compare, Allocator>::FillParentPointers(Node *root)
{
    if (!root)
        return;
    else{
        if (root->left_) root->left_->parent_ = root;
        if (root->right_) root->right_->parent_ = root;

        FillParentPointers(root->left_);
        FillParentPointers(root->right_);
    }
}

template<typename Type, typename Compare, typename Allocator>
typename AVLTree<Type,Compare,Allocator>::Node* AVLTree<Type, Compare, Allocator>::SmallLeftRotation(Node *a){
    // Задаём обозначения.
    Node* b = a->right_;
    Node* C = b->left_;

    // Переусыновляем вершины.
    a->right_ = C;
    if(C != nullptr)
        C->parent_ = a;
    b->left_ = a;
    a->parent_ = b;
    b->parent_ = nullptr;

    // Корректируем высоты в зависимости от того, равны ли высоты C и R.
    if (GetHeight(C) == GetHeight(b->right_)) {
        a->height_ -= 1;
        b->height_ += 1;
    } else {
        a->height_ -= 2;
    }
    return b;
}

template<typename Type, typename Compare, typename Allocator>
typename AVLTree<Type,Compare,Allocator>::Node* AVLTree<Type, Compare, Allocator>::SmallRightRotation(Node *a){
    // Задаём обозначения.
    Node* b = a->left_;
    Node* C = b->right_;

    // Переусыновляем вершины.
    a->left_ = C;
    if(C != nullptr)
        C->parent_ = a;
    b->right_ = a;
    a->parent_ = b;
    b->parent_ = nullptr;

    // Корректируем высоты в зависимости от того, равны ли высоты C и R.
    if (GetHeight(b->left_) == GetHeight(a->right_)) {
        a->height_ -= 1;
        b->height_ += 1;
    } else {
        a->height_ -= 2;
    }
    return b;
}

template<typename Type, typename Compare, typename Allocator>
size_t AVLTree<Type, Compare, Allocator>::GetHeight(Node *vertex){
    return  vertex ? vertex->height_ : 0;
}

template<typename Type, typename Compare, typename Allocator>
typename AVLTree<Type,Compare,Allocator>::Node* AVLTree<Type, Compare, Allocator>::BigLeftRotation(Node *v) {
    // Правым ребёнком становится новый корень правого поддерева.
    v->right_ = SmallRightRotation(v->right_);
    // Возвращаем новый корень поддерева.
    return SmallLeftRotation(v);
}

template<typename Type, typename Compare, typename Allocator>
typename AVLTree<Type,Compare,Allocator>::Node* AVLTree<Type, Compare, Allocator>::BigRightRotation(Node *v){
    v->left_  = SmallLeftRotation(v->left_);
    return SmallRightRotation(v);
}

template<typename Type, typename Compare, typename Allocator>
typename AVLTree<Type,Compare,Allocator>::Node* AVLTree<Type, Compare, Allocator>::Rotate(Node *vertex) {

    FixHeight(vertex);

    if (GetHeight(vertex->left_) - GetHeight(vertex->right_) == -2) {
        // Нам нужны левые вращения.
        Node* b = vertex->right_;
        Node* R = b->right_;
        Node* C = b->left_;

        if (GetHeight(C) <= GetHeight(R)) {
            // Нужно малое левое вращение.
            return SmallLeftRotation(vertex);
        } else {
            // Нужно большое левое вращение.
            return BigLeftRotation(vertex);
        }
    }
    if (GetHeight(vertex->left_) - GetHeight(vertex->right_) == 2){
        // Нам нужны правые вращения.
        Node* b = vertex->left_;
        Node* C = b->right_;
        Node* L = b->left_;

        if (GetHeight(C) <= GetHeight(L)) {
            // Нужно малое правое вращение.
            return SmallRightRotation(vertex);
        } else {
            // Нужно большое правое вращение.
            return BigRightRotation(vertex);
        }
    }
    return vertex;
}

template<typename Type, typename Compare, typename Allocator>
typename AVLTree<Type,Compare,Allocator>::Node* AVLTree<Type, Compare, Allocator>::InsertHelper(Node *vertex, const Type &value){
    if( !vertex ){
        ++size_;
        Node* node = AllocTraitsNode::allocate(alloc,1);
        AllocTraitsNode::construct(alloc,node,value);
        return node;
    }
    if(value<vertex->value_){
        vertex->left_ = InsertHelper(vertex->left_,value);
        vertex->left_->parent_ = vertex;
    }
    else if (value == vertex->value_)
        return vertex;
    else{
        vertex->right_ = InsertHelper(vertex->right_,value);
        vertex->right_ ->parent_ = vertex;
    }
    return Rotate(vertex);
}

template<typename Type, typename Compare, typename Allocator>
typename AVLTree<Type,Compare,Allocator>::Node* AVLTree<Type, Compare, Allocator>::TreeMin(Node *root)  {
    if(root->left_ == nullptr)
        return root;
    return TreeMin(root->left_);
}

template<typename Type, typename Compare, typename Allocator>
typename AVLTree<Type,Compare,Allocator>::Node* AVLTree<Type, Compare, Allocator>::TreeMax(Node *root) {
    if(root->right_ == nullptr)
        return root;
    return TreeMax(root->right_);
}
