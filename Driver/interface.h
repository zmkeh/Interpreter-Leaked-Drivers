typedef enum _request_codes
{
    request_base = 0x119,
    request_process_base = 0x199,
    request_read = 0x129,
    request_write = 0x139,
    request_success = 0x91a,
    request_unique = 0x92b,
}request_codes, *prequest_codes;

typedef struct _read_request {
    uint32_t pid;
    uintptr_t address;
    void *buffer;
    size_t size;
} read_request, *pread_request;

typedef struct _write_request {
    uint32_t pid;
    uintptr_t address;
    void *buffer;
    size_t size;
} write_request, *pwrite_request;

typedef struct _base_request {
    uint32_t pid;
    uintptr_t handle;
    const char* name;
} base_request, *pbase_request;

typedef struct _process_base_request {
    uint32_t pid;
    uintptr_t handle;
} process_base_request, * p_process_base_request;


typedef struct _request_data
{
    uint32_t unique;
    request_codes code;
    void *data;
}request_data, *prequest_data;
void main( int argc, char* argv[] )
{
    if ( !communication->attach( GetCurrentProcessId( ) ) )
    {
        printf( "failed to attatch to the process\n" );
        std::cin.get( );
    }
    if (!communication->is_mapped( ))
    {
        printf( "failed to initialize the driver.\n" );
        std::cin.get( );
    }

    printf( "finished operation\n" );

    std::cin.get( );

    uintptr_t base = communication->get_process_base( );

    printf( "base: %llx\n", base );

    std::cin.get( );

    printf( "test read: %llx\n", communication->read_physical_memory<uintptr_t>( base + test_ptr ) );

    std::cin.get( );

    printf( "writing to test ptr...\n" ); 

    communication->write_physical_memory<uintptr_t>( base + test_ptr, 0x69 );

    printf( "test read after write: %llx\n", communication->read_physical_memory<uintptr_t>( base + test_ptr ) );

    std::cin.get( );
     
    printf( "finished operation\n" );
}