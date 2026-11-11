#include "http.h"

#include <direct.h>
#include <errno.h>
#include <process.h>
#include <stdint.h>
#include <stdio.h>

int download_index(const char *url) {
  if (_mkdir("build") != 0 && errno != EEXIST) {
    perror("Cannot create build directory");
    return -1;
  }

  const char *arguments[] = {
      "curl.exe",     "--fail",     "--location", "--silent",
      "--show-error", "--max-time", "20",         "--output",
      INDEX_FILE,     "--",         url,          NULL};

  fflush(stdout);

  intptr_t status = _spawnvp(_P_WAIT, arguments[0], arguments);

  if (status == -1) {
    perror("Cannot start curl");
    remove(INDEX_FILE);
    return -1;
  }

  if (status != 0) {
    fprintf(stderr, "Download failed: curl returned %lld\n", (long long)status);
    remove(INDEX_FILE);
    return -1;
  }

  return 0;
}
