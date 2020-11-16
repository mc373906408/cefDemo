#ifndef HASHLIST_H
#define HASHLIST_H

#include <QList>
#include <QHash>
#include <QMap>

template <typename Key,typename T>
class HashList
{
public:
    HashList();

    void append(const Key &key,const T &value);
    void push_front(const Key &id,const T &value);// slow
    void push_back(const Key &id,const T &value);

    void removeAt(int i);// slow
    void remove(const Key &key);// slow
    void removeFirst();

    int size() const;
    int count() const;

    const T &at(int i) const;
    const T value(const Key &key) const;
    T take(const Key &key);

    T &operator[](const Key &key);
    const T operator[](const Key &key) const;

    QList<Key> keys() const;
    QList<Key> keysWithOrderliness() const;// slow

    T & first();
    const T &first() const;

    T &last();
    const T &last() const;

    bool contains(const Key &key) const;
private:
    QList<T> m_list;
    QHash<Key,int> m_hash;
};

template <typename Key,typename T>
HashList<Key,T>::HashList()
{
}

template <typename Key,typename T>
void HashList<Key,T>::push_front(const Key &id,const T &value)
{
    m_list.push_front(value);
    for (typename QHash<Key,T>::iterator it = m_hash.begin();
         it != m_hash.end(); ++it) {
        ++(it.value());
    }
    m_hash[id] = 0;
}

template <typename Key,typename T>
void HashList<Key,T>::push_back(const Key &id, const T &value)
{
    m_hash[id] = m_list.size();
    m_list.push_back(value);
}

template <typename Key,typename T>
void HashList<Key,T>::append(const Key &key,const T &value)
{
    push_back(key,value);
}

template <typename Key,typename T>
const T &HashList<Key,T>::at(int i) const
{
    return m_list.at(i);
}

template <typename Key,typename T>
const T HashList<Key,T>::value(const Key &key) const
{
    return m_list.at(m_hash.value(key));
}

template <typename Key,typename T>
T HashList<Key,T>::take(const Key &key)
{
    return m_list.at(m_hash.take(key));
}

template <typename Key,typename T>
void HashList<Key,T>::removeAt(int i)
{
    // build map of index to ID,it depends on orderliness of map
    QMap<int,Key> map;
    for (typename QHash<Key,int>::const_iterator it = m_hash.begin();
         it != m_hash.end(); ++it) {
        map[it.value()] = it.key();
    }

    // save the value is about to remove
    Key key = (map.begin() + i).value();

    // rebuild hash
    for (typename QMap<int,Key>::const_iterator it = map.begin() + i;
         it != map.end(); ++it) {
        m_hash[it.value()] = it.key() + 1;
    }

    m_hash.remove(key);
    m_list.removeAt(i);
}

template <typename Key,typename T>
int HashList<Key,T>::size() const
{
    return m_list.size();
}

template <typename Key,typename T>
int HashList<Key,T>::count() const
{
    return m_list.count();
}

template <typename Key,typename T>
void HashList<Key,T>::remove(const Key &key)
{
    removeAt(m_hash.value(key));
}

template <typename Key,typename T>
void HashList<Key,T>::removeFirst()
{
    Key key = m_list.first();
    m_list.removeFirst();
    m_hash.remove(key);
}

template <typename Key,typename T>
T & HashList<Key,T>::operator[](const Key &key)
{
    return m_list[m_hash.value(key)];
}

template <typename Key,typename T>
const T HashList<Key,T>::operator[](const Key &key) const
{
    return m_list[m_hash.value(key)];
}

template <typename Key,typename T>
QList<Key> HashList<Key,T>::keys() const
{
    return m_hash.keys();
}

template <typename Key,typename T>
QList<Key> HashList<Key,T>::keysWithOrderliness() const
{
    QMap<int,Key> map;
    for (typename QHash<Key,int>::const_iterator it = m_hash.begin();
         it != m_hash.end(); ++it) {
        map[it.value()] = it.key();
    }
    return map.keys();
}

template <typename Key,typename T>
T & HashList<Key,T>::first()
{
    return m_list.first();
}

template <typename Key,typename T>
const T &HashList<Key,T>::first() const
{
    return m_list.first();
}

template <typename Key,typename T>
T &HashList<Key,T>::last()
{
    return m_list.last();
}

template <typename Key,typename T>
const T &HashList<Key,T>::last() const
{
    return m_list.last();
}

template <typename Key,typename T>
bool HashList<Key,T>::contains(const Key &key) const
{
    return m_hash.contains(key);
}

#endif // HASHLIST_H
