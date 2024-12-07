/*
Copyright (C) 2006 Pedro Felzenszwalb

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

/* a simple map2dFloat class */

#pragma once

#include <cstring>

template <class T>
class map2dFloat {
public:
    /* create an map2dFloat */
    map2dFloat(const int width, const int height, const int precision, const bool init = true);

    /* delete an map2dFloat */
    ~map2dFloat();

    /* init an map2dFloat */
    void init(const T &val);

    /* wipe a map2d */
    void clear();

    /* print an map2dFloat */
    void print();

    /* copy an map2dFloat */
    map2dFloat<T> *copy() const;

    /* get the width of an map2dFloat. */
    int width() const {
        return w;
    }

    /* get the height of an map2dFloat. */
    int height() const {
        return h;
    }

    /* get the height of an map2dFloat. */
    T get(float x, float y) const {
        return access[int(y * precision)][int(x * precision)];
    }

    /* get the height of an map2dFloat. */
    void set(float x, float y, T value) const {
        access[int(y * precision)][int(x * precision)];
    }

    /* map2dFloat data. */
    T *data;

    /* row pointers. */
    T **access;

    int precision;
private:
    int w, h;
};

/* use imRef to access map2dFloat data. */
#define imRefFloat(im, x, y) (im->access[int(y * im->precision)][int(x * im->precision)])

/* use imPtr to get pointer to map2dFloat data. */
#define imPtrFloat(im, x, y) &(im->access[int(y * im->precision)][int(x * im->precision)])

template <class T>
map2dFloat<T>::map2dFloat(const int width, const int height, const int precision, const bool init) {
    w = width * precision;
    h = height * precision;
    data = new T[w * h];  // allocate space for map2dFloat data
    access = new T *[h];  // allocate space for row pointers

    // initialize row pointers
    for (int i = 0; i < h; i++)
        access[i] = data + (i * w);

    if (init)
        memset(data, 0, w * h * sizeof(T));
}

template <class T>
map2dFloat<T>::~map2dFloat() {
    delete[] data;
    delete[] access;
}

template <class T>
void map2dFloat<T>::print() {
    for (int y = 0; y < h; y++) {
        printf("|"); 
        for (int x = 0; x < w; x++) {
            printf("%2.1f|", access[y, x]);
        }
        printf("\n");
    }
}

template <class T>
void map2dFloat<T>::init(const T &val) {
    T *ptr = imPtr(this, 0, 0);
    T *end = imPtr(this, w - 1, h - 1);
    while (ptr <= end)
        *ptr++ = val;
}

template <class T>
void map2dFloat<T>::clear() {
    memset(data, 0, w * h * sizeof(T));
}

template <class T>
map2dFloat<T> *map2dFloat<T>::copy() const {
    map2dFloat<T> *im = new map2dFloat<T>(w, h, false);
    memcpy(im->data, data, w * h * sizeof(T));
    return im;
}