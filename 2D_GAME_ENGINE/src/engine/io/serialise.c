#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "serialise.h"
#include "../util.h"
#include "../types.h"

bool io_serialise_struct(const char* filepath, void* buffer, usize size, usize count) {
    assert(filepath && buffer && "passed null to function!");

    FILE* fp;
    fopen_s(&fp, filepath, "wb");
    if (!fp || ferror(fp))
        ERROR_RETURN(false, "failed to open file! %s %d", __FILE__, __LINE__);
    
    fwrite(buffer, size, count, fp);
    fclose(fp);
    
    return true;
}

bool io_deserialise_struct(const char* filepath, void* buffer, usize size, usize count) {
    assert(filepath && buffer && "passed null to function!");

    FILE* fp;
    fopen_s(&fp, filepath, "rb");
    if (!fp || ferror(fp))
        ERROR_RETURN(false, "failed to open file! %s %d", __FILE__, __LINE__);
 
    fread(buffer, size, count, fp);
    fclose(fp);
    
    return true;
}
