#include <systemc.h>
#include <occn.h>

// Producer module
SC_MODULE(Producer) {
	//declare the output port
    occn::MasterPort< occn::Pdu<char> > out;
    
    SC_CTOR(Producer) {
        SC_THREAD(main_thread);
    }
    
    void main_thread() {
        const char* str = "Hello OCCN!";
        int i = 0;
        
        while (str[i] != '\0') {
		//declare a protocol data unit carrying a char.
            occn::Pdu<char> *msg = new occn::Pdu<char>;

            *msg = str[i];

            cout << sc_time_stamp() << " Producer sending: " << str[i] << endl;
            
            bool sent;

            sc_time timeout(20, SC_NS);
	    //send via the occn output port
            out.send(msg, timeout, sent);
            
            if (!sent) {
                cout << "Failed to send character: " << str[i] << endl;
            }
            
            wait(10, SC_NS); // Small delay between sends

            i++;
        }
        sc_stop(); // Stop simulation when done
    }
};

// Consumer module
SC_MODULE(Consumer) {
    occn::SlavePort<occn::Pdu<char>> in;
    
    SC_CTOR(Consumer) {
        SC_THREAD(main_thread);
    }
    
    void main_thread() {

        sc_time timeout(50, SC_NS); // Create named timeout object
        
        while (true) {
            bool received;

            occn::Pdu<char>* msg = in.receive(timeout, received);
            
            if (received) {
                cout << sc_time_stamp() << " Consumer received: " << *msg << endl;
                in.reply(); // Acknowledge receipt
            } else {
                cout << sc_time_stamp() << " Consumer timeout" << endl;
            }
        }
    }
};

// Main program
int sc_main(int argc, char* argv[]) {
    // Create a clock
    sc_clock clock("clk", 10, SC_NS);
    
    // Create modules
    Producer prod("Producer");
    Consumer cons("Consumer");
    
    // Create and connect the channel
    occn::StdChannel<occn::Pdu<char>, occn::Pdu<char>> channel("Channel");
    channel.clk(clock);
    prod.out(channel);
    cons.in(channel);
    
    // Start simulation
    cout << "Starting simulation..." << endl;
    sc_start(); // Run until sc_stop() is called
    
    return 0;
}
