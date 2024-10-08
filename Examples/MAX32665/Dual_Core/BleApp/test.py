if __name__ == "__main__":
    # variable1 = ['{"SE":35,"evt":{"action":"scan completed"}}']
    # variable2 = ['{776:"0000","evt":{"handle":"1503","writeStatus":0}}']
    # variable3 = ['{777:"0000","evt":{"handle":"1502","hex":"0x0114000000400C0000270009404681","len":"15"}}']
    # variable4 = ['{"R":24,"evt":{"action":"scanning"}}']

    # # Initialize and parse each variable
    # # Your input string
    # input_str = str(variable3[0])

    # evt = input_str.find("\"evt\":")


    # parsable_str = input_str[evt+6:-1]
    # json_str = json.loads(parsable_str)

    # print(parsable_str)
    # print(json_str["handle"])
    # print(json_str["hex"])
    time_str = "C6000000650B0000"
    seconds = time_str[0:8]
    print(seconds)
    seconds = seconds[6:8] + seconds[4:6] + seconds[2:4] + seconds[0:2]
    print(seconds)

    subsec = time_str[9:16]
    print(subsec)
    subsec = subsec[6:8] + subsec[4:6] + subsec[2:4] + subsec[0:2]
    print(subsec)

    print(int(seconds,16),'.',int(subsec,16))
    