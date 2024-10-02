if __name__ == "__main__":
    variable1 = ['{"SE":35,"evt":{"action":"scan completed"}}']
    variable2 = ['{776:"0000","evt":{"handle":"1503","writeStatus":0}}']
    variable3 = ['{777:"0000","evt":{"handle":"1502","hex":"0x0114000000400C0000270009404681","len":"15"}}']
    variable4 = ['{"R":24,"evt":{"action":"scanning"}}']

    # Initialize and parse each variable
    # Your input string
    input_str = str(variable3[0])

    evt = input_str.find("\"evt\":")


    parsable_str = input_str[evt+6:-1]
    json_str = json.loads(parsable_str)

    print(parsable_str)
    print(json_str["handle"])
    print(json_str["hex"])