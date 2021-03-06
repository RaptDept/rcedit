// Copyright (c) 2013 GitHub, Inc. All rights reserved.
// Use of this source code is governed by MIT license that can be found in the
// LICENSE file.

#include <string.h>

#include "rescle.h"

bool print_error(const char* message) {
  fprintf(stderr, "Fatal error: %s\n", message);
  return 1;
}

bool parse_version_string(const wchar_t* str, unsigned short *v1, unsigned short *v2, unsigned short *v3, unsigned short *v4) {
  *v1 = *v2 = *v3 = *v4 = 0;
  if (swscanf_s(str, L"%hu.%hu.%hu.%hu", v1, v2, v3, v4) == 4)
    return true;
  if (swscanf_s(str, L"%hu.%hu.%hu", v1, v2, v3) == 3)
    return true;
  if (swscanf_s(str, L"%hu.%hu", v1, v2) == 2)
    return true;
  if (swscanf_s(str, L"%hu", v1) == 1)
    return true;

  return false;
}

int wmain(int argc, const wchar_t* argv[]) {
  bool loaded = false;
  rescle::ResourceUpdater updater;

  for (int i = 1; i < argc; ++i) {
    if (wcscmp(argv[i], L"--set-version-string") == 0 ||
        wcscmp(argv[i], L"-svs") == 0) {
      if (argc - i < 3)
        return print_error("--set-version-string requires 'Key' and 'Value'");

      const wchar_t* key = argv[++i];
      const wchar_t* value = argv[++i];
      if (!updater.SetVersionString(key, value))
        return print_error("Unable to change version string");

    } else if (wcscmp(argv[i], L"--set-file-version") == 0 ||
               wcscmp(argv[i], L"-sfv") == 0) {
      if (argc - i < 2)
        return print_error("--set-file-version requires a version string");

      unsigned short v1, v2, v3, v4;
      if (!parse_version_string(argv[++i], &v1, &v2, &v3, &v4))
        return print_error("Unable to parse version string");

      if (!updater.SetFileVersion(v1, v2, v3, v4))
        return print_error("Unable to change file version");

      if (!updater.SetVersionString(L"FileVersion", argv[i]))
        return print_error("Unable to change FileVersion string");

    } else if (wcscmp(argv[i], L"--set-product-version") == 0 ||
               wcscmp(argv[i], L"-spv") == 0) {
      if (argc - i < 2)
        return print_error("--set-product-version requires a version string");

      unsigned short v1, v2, v3, v4;
      if (!parse_version_string(argv[++i], &v1, &v2, &v3, &v4))
        return print_error("Unable to parse version string");

      if (!updater.SetProductVersion(v1, v2, v3, v4))
        return print_error("Unable to change file version");

      if (!updater.SetVersionString(L"ProductVersion", argv[i]))
        return print_error("Unable to change ProductVersion string");

    } else if (wcscmp(argv[i], L"--set-icon") == 0 ||
               wcscmp(argv[i], L"-si") == 0) {
      if (argc - i < 2)
        return print_error("--set-icon requires path to the icon");

      if (!updater.SetIcon(argv[++i]))
        return print_error("Unable to set icon");
    } else if (wcscmp(argv[i], L"--set-resource-string") == 0 ||
      wcscmp(argv[i], L"--srs") == 0) {
      if (argc - i < 3)
        return print_error("--set-resource-string requires int 'Key' and string 'Value'");

      const wchar_t* key = argv[++i];
      unsigned int key_id = 0;
      if (swscanf_s(key, L"%d", &key_id) != 1)
        return print_error("Unable to parse id");

      const wchar_t* value = argv[++i];
      if (!updater.ChangeString(key_id, value))
        return print_error("Unable to change string");
    } else {
      if (loaded)
        return print_error("Unexpected trailing arguments");

      loaded = true;
      if (!updater.Load(argv[i]))
        return print_error("Unable to load file");

    }
  }

  if (!loaded)
    return print_error("You should specify a exe/dll file");

  if (!updater.Commit())
    return print_error("Unable to commit changes");

  return 0;
}
