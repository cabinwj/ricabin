/*
 * Copyright (C) 2005,2006,2007 MaNGOS <http://www.mangosproject.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _COMMON_SINGLETON_H_
#define _COMMON_SINGLETON_H_

#include "common_types.h"


template<typename T>
class object_singleton
{
public:
    static T& Instance()
    {
        if (NULL == m_instance_)
        {
            m_instance_ = new T;
        }

        return *m_instance_;
    }

protected:
    object_singleton() {}

private:
    // Prohibited actions...this does not prevent hijacking.
    object_singleton(const object_singleton&);
    object_singleton& operator=(const object_singleton&);

    static T* m_instance_;
};

template<typename T>
T* object_singleton<T>::m_instance_ = NULL;

#endif  // _COMMON_SINGLETON_H_
