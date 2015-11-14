/*
 * Copyright (c) 2014, 2015, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#ifndef SHARE_VM_CLASSFILE_CLASSLOADEREXT_HPP
#define SHARE_VM_CLASSFILE_CLASSLOADEREXT_HPP

#include "classfile/classLoader.hpp"
#include "oops/instanceKlass.hpp"
#include "runtime/handles.hpp"

class ClassLoaderExt: public ClassLoader { // AllStatic
public:

  class Context {
    const char* _file_name;
  public:
    Context(const char* class_name, const char* file_name, TRAPS) {
      _file_name = file_name;
    }

    bool check(ClassFileStream* stream, const int classpath_index) {
      return true;
    }

    bool should_verify(int classpath_index) {
      return false;
    }

    instanceKlassHandle record_result(const int classpath_index,
                                      const jshort classloader_type,
                                      ClassPathEntry* e, instanceKlassHandle result, TRAPS) {
      if (ClassLoader::add_package(_file_name, classpath_index, THREAD)) {
        if (DumpSharedSpaces) {
          result->set_shared_classpath_index(classpath_index);
          result->set_class_loader_type(classloader_type);
        }
        return result;
      } else {
        return instanceKlassHandle(); // NULL
      }
    }
  };


  static void add_class_path_entry(const char* path, bool check_for_duplicates,
                                   ClassPathEntry* new_entry, bool prepend_entry) {
    if (prepend_entry) {
      ClassLoader::prepend_to_list(new_entry);
    } else {
      ClassLoader::add_to_list(new_entry);
    }
  }
  static void append_boot_classpath(ClassPathEntry* new_entry) {
    ClassLoader::add_to_list(new_entry);
    // During jvmti live phase an entry can be appended to the boot
    // loader's ClassPathEntry instances.  Need to mark the start
    // of the boot loader's append path in case there was no reason
    // to mark it initially in setup_bootstrap_search_path.
    if (ClassLoader::_first_append_entry == NULL) {
      ClassLoader::set_first_append_entry(new_entry);
    }
  }
  static void setup_search_paths() {}
};

#endif // SHARE_VM_CLASSFILE_CLASSLOADEREXT_HPP
