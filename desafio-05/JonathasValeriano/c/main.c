#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <fcntl.h>

#include <windows.h>
#include <process.h>
#include <intrin.h>

typedef struct
{
  uintptr_t thread;
  const char* start;
  long long size;

} chunk_t;

int get_nprocs()
{
  return getenv("NUMBER_OF_PROCESSORS") ? atoi(getenv("NUMBER_OF_PROCESSORS")) : 1;
}

size_t page_size()
{
  SYSTEM_INFO si;
  ZeroMemory(&si, sizeof(si));
  GetSystemInfo(&si);
  return si.dwPageSize;;
}

size_t make_offset_page_aligned(size_t offset)
{
  const size_t page_size_ = page_size();
  return offset / page_size_ * page_size_;
}

void worker(chunk_t* chunk)
{
  const char* cp = chunk->start;
  const char* end = chunk->start + chunk->size;
  const char* nome;
  const char* sobrenome;
  const char* salario;
  const char* area;
  char lch = 0;
  int quoteCount = 0;
  int divideCount = 0;
  const char *p;
  int id;
  char buf[100];

  while( (cp = strstr(cp + 1, "\"i")) && cp < end )
  {
    id = atoi(cp += 5);
    sprintf(buf, "id=%d\n", id);
    fputs(buf, stdout);

    //cp = strstr(cp, "\"n" );
    //continue;

    /*
    if( p = strstr(cp, "\"n" ) )
    {
      quoteCount++;
      cp = p + 2;
    }
    else if( p = strstr(cp, "\"s" ) )
    {
      quoteCount++;
      cp = p + 2;
    }
    else if( p = strstr(cp, "\"s" ) )
    {
      quoteCount++;
      cp = p + 2;
    }
    else if( p = strstr(cp, "\"a" ) )
    {
      quoteCount++;
      cp = p + 2;
    }
    */
  }

  //printf("%d quotes, %d divides\n", quoteCount, divideCount);
}

int main(int argc, char* argv[])
{
  int num_threads;
  HANDLE file;
  size_t offset = 0;
  long long aligned_offset;
  long long length_to_map;
  char* mapping_start;

  if( argc != 2 )
  {
    return 1;
  }

  num_threads = get_nprocs();
  file = CreateFileA(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL /*| FILE_FLAG_SEQUENTIAL_SCAN*/, NULL);

  if (file == INVALID_HANDLE_VALUE)
  {
    return 1;
  }

  LARGE_INTEGER li_file_size;
  GetFileSizeEx(file, &li_file_size);
  const size_t file_size = li_file_size.QuadPart;

  aligned_offset = make_offset_page_aligned(offset);
  length_to_map = offset - aligned_offset + file_size;

  HANDLE file_mapping = CreateFileMapping(file, NULL, PAGE_READONLY, li_file_size.HighPart, li_file_size.LowPart, NULL);
  if (file_mapping == NULL)
  {
    return 1;
  }

  mapping_start = (char*) MapViewOfFile(file_mapping,FILE_MAP_READ, 0, 0, length_to_map);

  if (mapping_start)
  {
    char* p = mapping_start;
    int nprocs = get_nprocs();
    long long chunk_size = length_to_map / nprocs;
    chunk_t* threads = calloc(nprocs, sizeof(chunk_t));
    HANDLE *threads_handles = calloc(nprocs, sizeof(HANDLE));
    int i;

    /*
    if (p = strstr(mapping_start, "\"funcionarios\""))
    {
      (char*)p += 16;
    }
    */

    if (threads && threads_handles)
    {
      for (i = 0; i < nprocs; ++i, p += chunk_size)
      {
        threads[i].start = p;
        threads[i].size = chunk_size;
        threads_handles[i] = threads[i].thread = (HANDLE) _beginthread(worker, 0, &threads[i]);
      }


      WaitForMultipleObjects(nprocs, threads_handles, TRUE, INFINITE);

      //for (i = 0; i < nprocs; ++i, p += chunk_size)
      //{
      //  WaitForSingleObject((HANDLE) threads[i].thread, INFINITE);
      //}

    }

    UnmapViewOfFile(mapping_start);
  }

  CloseHandle(file_mapping);
  CloseHandle(file);
}
