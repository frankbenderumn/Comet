#include "comet/command/purge.h"

int main(int argc, char* argv[]) {

    // fixtures
    SemVer* sv = (SemVer*)malloc(sizeof(SemVer));
    PackageManifest* pm = (PackageManifest*)malloc(sizeof(PackageManifest));
    sv->major = 0;
    sv->minor = 0; 
    sv->patch = 1;
    strncpy(sv->string, "0.0.1", 5);
    sv->string[5] = 0;
    strncpy(pm->name, "prizm", 5);
    pm->name[5] = 0;
    strncpy(pm->repo, "Prizm-cxx", 9);
    pm->repo[9] = 0;
    strncpy(pm->uri, "https://frankbenderumn:${GITHUB_TOKEN}@github.com/frankbenderumn/Prizm-cxx.git", 78); 
    pm->uri[78] = 0;
    strncpy(pm->source, "github-clone", 12);
    pm->source[12] = 0;
    pm->latest = sv;

    if (uri_env(pm->uri, pm->name, pm->latest->string) < 0) {
        BRED("Error interpolating URI\n");
    }

    manifest_dump(pm);

    // purge(pm);

    return 0;
    
}