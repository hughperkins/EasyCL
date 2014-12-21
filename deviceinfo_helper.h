#pragma once

void printDeviceInfoKB( string valuename, cl_device_id deviceId, cl_device_info name ) {
    cl_ulong somelong = 0;
    clGetDeviceInfo(deviceId, name, sizeof(cl_ulong), &somelong, 0);
    cout << valuename << ": " << ( somelong/1024 ) << "KB" << endl;    
}

void printDeviceInfoMB( string valuename, cl_device_id deviceId, cl_device_info name ) {
    cl_ulong somelong = 0;
    clGetDeviceInfo(deviceId, name, sizeof(cl_ulong), &somelong, 0);
    cout << valuename << ": " << ( somelong/1024/1024 ) << "MB" << endl;    
}

void printDeviceInfo( string valuename, cl_device_id deviceId, cl_device_info name ) {
    cl_ulong somelong = 0;
    clGetDeviceInfo(deviceId, name, sizeof(cl_ulong), &somelong, 0);
    cout << valuename << ": " << somelong << endl;    
}

void printDeviceInfoArray( string valuename, cl_device_id deviceId, cl_device_info name, int length ) {
    cl_ulong *array = new cl_ulong[length];
    clGetDeviceInfo(deviceId, name, sizeof(cl_ulong)*length, array, 0);
    cout << valuename << ":";
    for( int i = 0; i < length; i++ ) {
        cout << " " << array[i];   
    }
    cout << endl;
    delete[] array;
}

void printDeviceInfoString( string valuename, cl_device_id deviceId, cl_device_info name ) {
    char buffer[256];
    clGetDeviceInfo(deviceId, name, 256, buffer, 0);
    cout << valuename << ": " << buffer << endl;    
}


