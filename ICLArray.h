class ICLArray {
public:
    virtual ~ICLArray(){}

    virtual cl_mem *getDeviceArray() = 0;
    virtual void copyToDevice() = 0;
    virtual void copyToHost() = 0;
    virtual int size() = 0;
};

