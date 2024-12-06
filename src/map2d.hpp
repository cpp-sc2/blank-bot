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

/* a simple image class */

#ifndef MAP_H
#define MAP_H

#include <cstring>

template <class T>
class map2d {
public:
    /* create an image */
    map2d(const int width, const int height, const bool init = true);

    /* delete an image */
    ~map2d();

    /* init an image */
    void init(const T &val);

    /* wipe a map2d */
    void clear();

    /* print an image */
    void print();

    /* copy an image */
    map2d<T> *copy() const;

    /* get the width of an image. */
    int width() const {
        return w;
    }

    /* get the height of an image. */
    int height() const {
        return h;
    }

    /* image data. */
    T *data;

    /* row pointers. */
    T **access;

private:
    int w, h;
};

/* use imRef to access image data. */
#define imRef(im, x, y) (im->access[y][x])

/* use imPtr to get pointer to image data. */
#define imPtr(im, x, y) &(im->access[y][x])

template <class T>
map2d<T>::map2d(const int width, const int height, const bool init) {
    w = width;
    h = height;
    data = new T[w * h];  // allocate space for image data
    access = new T *[h];  // allocate space for row pointers

    // initialize row pointers
    for (int i = 0; i < h; i++)
        access[i] = data + (i * w);

    if (init)
        memset(data, 0, w * h * sizeof(T));
}

template <class T>
map2d<T>::~map2d() {
    delete[] data;
    delete[] access;
}

template <class T>
void map2d<T>::print() {
    for (int y = 0; y < h; y++) {
        printf("|"); 
        for (int x = 0; x < w; x++) {
            printf("%2.1f|", access[y, x]);
        }
        printf("\n");
    }
}

template <class T>
void map2d<T>::init(const T &val) {
    T *ptr = imPtr(this, 0, 0);
    T *end = imPtr(this, w - 1, h - 1);
    while (ptr <= end)
        *ptr++ = val;
}

template <class T>
void map2d<T>::clear() {
    memset(data, 0, w * h * sizeof(T));
}

template <class T>
map2d<T> *map2d<T>::copy() const {
    map2d<T> *im = new map2d<T>(w, h, false);
    memcpy(im->data, data, w * h * sizeof(T));
    return im;
}

#endif
