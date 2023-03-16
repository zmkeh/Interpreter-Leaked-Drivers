#pragma warning(disable  : 4996).

#include "../km/interface.h"

class kcommunication_t
{
private:
    uint64_t( __fastcall* dolphin_invoke )(int64_t a1, uint32_t a2, uint64_t a3, uint64_t a4, uint64_t a5, uint32_t a6, uint32_t a7);

    HWND hwnd;
    int pid = 0;
public:

    inline bool attach( int _pid )
    {
        if (!_pid)
            return false;

        pid = _pid;

        return true;
    }

    inline auto send_cmd( void* data, request_codes code ) -> bool
    {
        if (!data || !code)
        {
            return false;
        }

        request_data request{ 0 };

        request.unique = request_unique;
        request.data = data;
        request.code = code;

        const auto result = dolphin_invoke( ( int64_t ) hwnd, 0x404, ( uintptr_t ) &request, 0, 0, 221, 0 );

        if (result != request_success)
        {
            return false;
        }

        return true;
    }

    inline auto get_module_base( const char* module_name ) -> const std::uintptr_t
    {
        base_request data{ 0 };

        data.pid = pid;
        data.handle = 0;
        data.name = module_name;

        send_cmd( &data, request_base );

        return data.handle;
    }

    inline auto get_process_base( ) -> const std::uintptr_t
    {
        process_base_request data{ 0 };

        data.pid = pid;
        data.handle = 0;

        send_cmd( &data, request_process_base );

        return data.handle;
    }

    inline auto is_mapped( ) -> bool
    {
        LoadLibraryA( ( "win32u.dll" ) );

        HMODULE win32u = GetModuleHandleA( ( "win32u.dll" ) );
        while ( !win32u )
        {
            win32u = GetModuleHandleA( ( "win32u.dll" ) );
            Sleep( 15 );
        }

        *( PVOID* ) &dolphin_invoke = GetProcAddress( win32u, (( "NtUserMessageCall" )) );
        hwnd = GetDesktopWindow( );

        if ( !dolphin_invoke )
            return false;

        int old_pid = pid;
        attach( GetCurrentProcessId( ) );
        bool com_success = get_module_base( ( "win32u.dll" ) ) == ( uintptr_t ) win32u;
        attach( old_pid );

        return com_success;
    }

    inline auto read_physical_memory( const std::uintptr_t address, void* buffer, const std::size_t size ) -> bool
    {
        read_request data{ 0 };

        data.pid = pid;
        data.address = address;
        data.buffer = buffer;
        data.size = size;

        return send_cmd( &data, request_read );
    }

    template <typename t>
    inline auto read_physical_memory( const std::uintptr_t address ) -> t
    {
        t response{ };
        read_physical_memory( address, &response, sizeof( t ) );
        return response;
    }

    inline auto write_physical_memory( const std::uintptr_t address, void* buffer, const std::size_t size ) -> bool
    {
        write_request data{ 0 };

        data.pid = pid;
        data.address = address;
        data.buffer = buffer;
        data.size = size;

        return send_cmd( &data, request_write );
    }

    template <typename t>
    inline auto write_physical_memory( const std::uintptr_t address, t value ) -> bool
    {
        return write_physical_memory( address, &value, sizeof( t ) );
    }
};

static kcommunication_t* communication = new kcommunication_t( );
