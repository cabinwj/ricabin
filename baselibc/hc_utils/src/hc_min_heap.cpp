#include "hc_min_heap.h"
#include "hc_log.h"


int min_heap::push(min_helem* helem)
{
    if (0 != reserve())
    {
        return -1;
    }

    shift_up(m_count_++, helem);
    return 0;
}

min_helem* min_heap::pop()
{
    if (0 != m_count_)
    {
        min_helem* helem = *m_array_;
        shift_down(0, m_array_[--m_count_]);
        helem->min_heap_idx(-1);
        return helem;
    }

    return 0;
}

int min_heap::erase(min_helem* helem)
{
    if (-1 != helem->min_heap_idx())
    {
        min_helem* last = m_array_[--m_count_];
        int32_t parent_idx = (helem->min_heap_idx() - 1) / 2;
        /* we replace helem with the last element in the heap.  We might need to
            shift it upward if it is less than its parent, or downward if it is
            greater than one or both its children. Since the children are known
            to be less than the parent, it can't need to shift both up and
            down. */
        if (helem->min_heap_idx() > 0 && (m_array_[parent_idx]->weight() > last->weight()))
        {
            shift_up(helem->min_heap_idx(), last);
        }
        else
        {
            shift_down(helem->min_heap_idx(), last);
        }

        helem->min_heap_idx(-1);

        return 0;
    }

    return -1;
}

int min_heap::reserve()
{
    int32_t count = m_count_ + 1;
    if (count < m_capacity_)
    {
        int32_t capacity = m_capacity_ ? m_capacity_ * 2 : 8;
        if (capacity < count)
        {
            capacity = count;
        }

        min_helem** array_ptr =  NULL;
        if (NULL == (array_ptr = (min_helem**)allocatorc::Instance()->Allocate(capacity * sizeof(min_helem*))))
        {
            return -1;
        }

        for (int i = 0; i < m_count_; i++)
        {
            *(array_ptr + i) = *(m_array_ + i);
        }
        
        m_array_ = array_ptr;
        m_capacity_ = capacity;
    }

    return 0;
}

void min_heap::shift_up(int32_t hole_idx, min_helem* helem)
{
    int32_t parent_idx = (hole_idx - 1) / 2;
    while (hole_idx && (m_array_[parent_idx]->weight() > helem->weight()))
    {
        m_array_[hole_idx] = m_array_[parent_idx];
        m_array_[hole_idx]->min_heap_idx(hole_idx);
        hole_idx = parent_idx;
        parent_idx = (hole_idx - 1) / 2;
    }
    m_array_[hole_idx] = helem;
    m_array_[hole_idx]->min_heap_idx(hole_idx);
}

void min_heap::shift_down(int32_t hole_idx, min_helem* helem)
{
    int32_t min_child_idx = 2 * (hole_idx + 1);
    while (min_child_idx <= m_count_)
    {
        min_child_idx -= min_child_idx == m_count_ || (m_array_[min_child_idx]->weight() > m_array_[min_child_idx - 1]->weight());
        if (!(helem->weight() > m_array_[min_child_idx]->weight()))
        {
            break;
        }

        m_array_[hole_idx] = m_array_[min_child_idx];
        m_array_[hole_idx]->min_heap_idx(hole_idx);
        hole_idx = min_child_idx;
        min_child_idx = 2 * (hole_idx + 1);
    }
    m_array_[hole_idx] = helem;
    m_array_[hole_idx]->min_heap_idx(hole_idx);
}
