import serial
import struct
import datetime

datasize = 2 * 250 * 20
databuf = [0] * datasize


def main():
    ser = serial.Serial('/dev/ttyUSB0', 460800)

    # datasize = 10
    print(datasize)

    ser.write(str.encode('connect'))

    for num in range(datasize):
        while True:
            if ser.in_waiting > 0:
                break
        recv_data = ser.read(1)
        databuf[num] = int.from_bytes(recv_data, 'big')
    #        print("{0}".format(databuf[num]))

    #    print("********")
    filename = 'log/soundlog' + datetime.datetime.now().strftime('%Y%m%d%H%M%S') + '.csv'
    with open(filename, "wb") as f:
        f.write(str.encode("Sound\n"))
        for num in range(0, datasize, 2):
            data = databuf[num] + (databuf[num + 1] << 8)
            #            print("{0}".format(data))
            f.write(str.encode("{0}\n".format(data)))


if __name__ == '__main__':
    main()
