#include <iostream>
#include <optional>
#include <map>
#include <cassert>
#include <random>
#include <thread>

template<typename K, typename V>
class interval_map
{
    friend void IntervalMapTest();
    V m_valBegin;
    std::map<K,V> m_map;
public:
    // constructor associates whole range of K with val
    interval_map(V const& val) : m_valBegin(val) {}

    // Assign value val to interval [keyBegin, keyEnd).
    // Overwrite previous values in this interval.
    // Conforming to the C++ Standard Library conventions, the interval
    // includes keyBegin, but excludes keyEnd.
    // If !( keyBegin < keyEnd ), this designates an empty interval,
    // and assign must do nothing.
    void assign(K const& keyBegin, K const& keyEnd, V const& val)
    {
        if (!(keyBegin < keyEnd))
        {
            // an empty interval
            // I suppose that "do nothing" from the comment above means just exit the function, so I did this
            return;
        }
        if (m_map.empty() && val == m_valBegin)
        {
            // the first entry in m_map must not contain the same value as m_valBegin
            // it is not specificed how to handle this usecase, so I'll just throw exception
            throw std::runtime_error("first entry in map must not contain same value as m_valBegin");
        }

        // just for checks, to avoid additional calls
        const auto cMapEnd = m_map.cend();

        // get the lower boudns of begin & end keys
        auto beginLowerBound = m_map.lower_bound(keyBegin);
        auto endLowerBound = m_map.lower_bound(keyEnd);

        // check is there a duplicate pair before potential range
        if (beginLowerBound != cMapEnd && beginLowerBound != m_map.cbegin())
        {
            auto beforeBeginLowerBound = beginLowerBound;
            beforeBeginLowerBound--;
            if (beforeBeginLowerBound->second == val)
            {
                // The representation in the std::map must be canonical, that is,
                // consecutive map entries must not contain the same value: ..., (3,'A'), (5,'A'), ... is not allowed.
                // it is not specificed how to handle this usecase, so I'll just throw exception
                throw std::runtime_error("consecutive map entries must not contain the same value");
            }
        }
        // check is there a duplicate pair after potential range
        if (endLowerBound != cMapEnd)
        {
            auto afterEndLowerBound = endLowerBound;
            afterEndLowerBound++;
            if (afterEndLowerBound->second == val)
            {
                // The representation in the std::map must be canonical, that is,
                // consecutive map entries must not contain the same value: ..., (3,'A'), (5,'A'), ... is not allowed.
                // it is not specificed how to handle this usecase, so I'll just throw exception
                throw std::runtime_error("consecutive map entries must not contain the same value");
            }
        }

        // optional ending range pair
        std::optional<std::pair<K,V>> additionalElement;

        if (endLowerBound == cMapEnd || keyEnd < endLowerBound->first)
        {
            // see who shall be the next neigbhour
            char endVal = this->operator[](keyEnd);
            if (!(endVal == val))
            {
                // it is forbidden to have a two in a row same pairs,
                // but it is not specified how to handle this usecase, so I'll just extend the range
                additionalElement = std::pair(keyEnd, endVal);
            }
        }
        else if (beginLowerBound == cMapEnd || keyBegin < beginLowerBound->first)
        {
            // see who shall be the next neigbhour
            char endVal = this->operator[](keyBegin);
            if (!(endVal == val))
            {
                // it is forbidden to have a two in a row same pairs,
                // but it is not specified how to handle this usecase, so I'll just extend the range
                additionalElement = std::pair(keyEnd, endVal);
            }
        }

        // erase all pairs within new range
        if (beginLowerBound != cMapEnd)
        {
            m_map.erase(beginLowerBound, endLowerBound);
        }

        // insert new range
        m_map.insert(std::pair(keyBegin, val));

        // if it exist add ending range pair
        if (additionalElement)
        {
            m_map.insert(additionalElement.value());
        }

        /// I just wan't to emphasize that a lot of things are not explained & specified so,
        // please, take a look if some usecases fail because of different expectations.
    }

    // look-up of the value associated with key
    V const& operator[]( K const& key ) const
    {
        auto it=m_map.upper_bound(key);
        if(it==m_map.begin())
        {
            return m_valBegin;
        }
        else
        {
            return (--it)->second;
        }
    }
    
    void printIntervalMap() const
    {
        std::cout << "begin val = " << m_valBegin << std::endl;
        for (auto it = m_map.cbegin(); it != m_map.cend(); it++)
        {
            std::cout << "[" << it->first << "] = " << it->second << std::endl;
        }
    }
};

void IntervalMapTest()
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<char> distAscii('!', '~');
    std::uniform_int_distribution<int> distNum(-150, 150);
    interval_map<int, char> M(distAscii(rng));

    for (int i=0; i<3500; i++)
    {
        bool exception = false;
        const int beginKey = distNum(rng);
        const int endKey = distNum(rng);
        const char val = distAscii(rng);
        
        const char currBeginKeyVal = M[beginKey];
        const char currEndKeyVal = M[endKey];

        try
        {
            M.assign(beginKey, endKey, val);
        }
        catch (...)
        {
            exception = true;
            continue;
        }

        assert(M[endKey] == currEndKeyVal);
        if (!(beginKey < endKey) || exception)
        {
            assert(M[beginKey] == currBeginKeyVal);
        }
        else
        {
            assert(M[beginKey] == val);
        }
    }
}

int main()
{
    IntervalMapTest();

    return 0;
}
