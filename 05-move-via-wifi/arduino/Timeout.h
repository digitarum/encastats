class Timeout {
    
private:
    unsigned long startTime;
public:
    Timeout(void) {};
    void Reset();
    bool HasElapsed(const unsigned int delay);
};
