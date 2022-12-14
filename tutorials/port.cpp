// Learn with Examples, 2020, MIT license
#include <systemc>
using namespace sc_core;

SC_MODULE(MODULE_PORTWRITE)
{                                     // defines one module
    sc_signal<int> s;                 // a signal (channel) inside the module
    sc_port<sc_signal_out_if<int>> p; // a port used to write to an outside channel
    SC_CTOR(MODULE_PORTWRITE)
    {
        SC_THREAD(selfWrite); // a process to write to own channel
        SC_THREAD(selfRead);  // a process to read from own channel
        sensitive << s;       // triggered by value change on the channel
        dont_initialize();
        SC_THREAD(outsideWrite); // a process to write to an outside channel
    }
    void selfWrite()
    {
        int val = 1; // init value
        while (true)
        {
            s.write(val++);  // write to own channel
            wait(1, SC_SEC); // repeat after 1 s
        }
    }
    void selfRead()
    {
        while (true)
        {
            std::cout << sc_time_stamp() << ": reads from own channel, val=" << s.read() << std::endl; // read from own channel
            wait();                                                                                    // receives from signal
        }
    }
    void outsideWrite()
    {
        int val = 1; // init value
        while (true)
        {
            p->write(val++); // write to an outside channel, calls the write method of the outside channel. p is a pointer.
            wait(1, SC_SEC);
        }
    }
};
SC_MODULE(MODULE_PORTREAD)
{                                    // a module that reads from an outside channel
    sc_port<sc_signal_in_if<int>> p; // a port used to read from an outside channel
    SC_CTOR(MODULE_PORTREAD)
    {
        SC_THREAD(outsideRead); // a process to read from an outside channel
        sensitive << p;         // triggered by value change on the channel
        dont_initialize();
    }
    void outsideRead()
    {
        while (true)
        {
            std::cout << sc_time_stamp() << ": reads from outside channel, val=" << p->read() << std::endl; // use port to read from the channel, like a pointer.
            wait();                                                                                         // receives from port
        }
    }
};

int sc_main(int, char *[])
{
    MODULE_PORTWRITE module_portwrite("port_writer"); // instantiate module1
    MODULE_PORTREAD module_portread("port_reader");   // instantiate module2
    sc_signal<int> s;                                 // declares a signal (channel) outside module1 and moudle2
    module_portwrite.p(s);                            // binds (connects) port p of module1 to channel (signal) s
    module_portread.p(s);                             // binds port p of module2 to channel s
    sc_start(2, SC_SEC);
    return 0;
}
