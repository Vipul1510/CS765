contract fact_Checking_DApp{
    // address of the owner
    address payable public owner;
    // Value of registration fee
    uint256 public RegistrationFee;
    // value of the news fee
    uint256 public NewsFee;
    // Map which keeps track of peers who are fact checker (after successful registration)
    mapping(address => Voter) public factCheckers;
    // Map which keeps track of Valid KYC among all possible combinations updated by bank as of now
    // mapping(uint256 => bool) public Valid_KYC;
    // Map which keeps track of KYC ID used by each user
    // mapping(uint256 => address) public KYC_mapping;
    // Map which keep track of hash of news to original news item (efficiency is increased)
    mapping(bytes32 => News_item) public news_items;
    // Variable which keep tracks of Number of news items till now which are verified
    uint256 NUM_NEWS;
    // Precision variable used in function "check_news()"
    uint256 PRECISION;

    // constructor for contract
    constructor(uint256 regFees, uint256 newsfees){
        owner = payable(msg.sender);
        RegistrationFee = regFees;
        NewsFee = newsfees
        NUM_NEWS = 0;
        PRECISION = 8192;
    }

    // Struct for a news item
    struct News_item{
        address sender;         // address of the sender
        string content;         // Content (news) which is to be verified
        bool is_fake;           // Final prediction
        uint8 news_type;        // Type of news (MAX 256 are possible in implementation)
        uint256 Score;          // Variable is updated for each vote of voter
        uint256 num_voters;     // Number of voters who have given their vote
        mapping(address => bool) votes; // Keeps track of vote given by each voter, it's used while updating the trustworthiness after calculating majority
    }

    // Struct for a voter
    struct Voter{
        bool is_registered;         // Bool which tells registration is done or not
        address = voter_address;    // Stores address of the voter
        uint256 balance = 0;        // Balance of the voter at given instance
        mapping(uint8 => uint8) TrustWorthiness;    // Mapping which keeps track of trustworthiness for each type of news item
        mapping(uint8 => uint8) TrustCount;         // Mapping which keeps track of trust count for each type of news item
        mapping(bytes32 => bool) previous_votes;    // Mapping which keeps track of past votes given by voter
    }

    // Function to register a user as Fast Checker
    // function register_as_factChecker(uint256 kyc_id) external payable {
    function register_as_factChecker() external payable {
        // require(Valid_KYC[kyc_id] == true , "KYC ID is invalid");
        // require(KYC_mapping[kyc_id] == false , "KYC ID is used already");
        // require(factCheckers[msg.sender].is_registered == false, "Address already registered");
        // Updating required variables
        // KYC_mapping[kyc_id] = true;
        require(msg.value == registrationFee, "Incorrect registration fees");
        require(!(factCheckers[msg.sender].is_registered), "User already registered");
        factCheckers[msg.sender].is_registered = true;
        factCheckers[msg.sender].voter_address = msg.sender;
        // Initialising trustworthiness and trust count for each news type
        for (uint8 i = 0; i < 256; i++) {
            factCheckers[msg.sender].TrustWorthiness[i] = 128;
            fastCheckers[msg.sender].TrustCount[i] = 0;
        }

    }

     // Function to submit a news item for fact-checking
    function request_for_news_check(string memory news, uint8 type_of_news) external payable {
        require(msg.value == NewsFees, "Incorrect News fees");
        bytes32 item = keccak256(abi.encodePacked(news, msg.sender, block.timestamp));
        require(news_items[item].sender == address(0), "News item is already submitted for fact checking");
        // Create a news item at it's hash value
        news_items[item] = News_item({
            sender: msg.sender,
            content: news,
            news_type: type_of_news
        });
    }

    // Function to vote for a given News Item
    function vote_for_newsitem(bytes32 item_id, bool vote_assgn) public {
        require(factCheckers[msg.sender].is_registered == true , "Only registered fact-checkers can vote");
        News_item storage item = news_items[item_id];
        // Assign the vote
        item.votes[msg.sender] = vote_assgn;
        // Update the score of the news and other variables
        if(vote_assgn == true)
            item.Score += factCheckers[msg.sender].TrustWorthiness[item.news_type];
        item.num_voters+=1;
        // Store vote in history of voter
        previous_votes[item_id] = vote_assgn;

    }

    // Function which gives prediction/ result of given news item
    function check_news(bytes32 item_id) public view return (bool){
        News_item storage item = news_items[item_id];
        require(item.sender != address(0), "News item does not exist");
        NUM_NEWS +=1;
        bool news_pred = item.Score > item.num_voters/2;
        for (uint256 i = 0; i < addressList.length; i++) {
            address voter = addressList[i];
            if(voter.previous_votes[item_id] == news_pred){
                // Increase Trustworthiness of that user
                voter.TrustCount[item.news_type] += 1;
                // Incentive for voter who favours majority
                voter.balance += NewsFee / (item.num_voters);
            }
            // See report for this update
            uint32 factor = PRECISION/((NUM_NEWS/8) + 1)
            voter.TrustWorthiness[item.news_type] = (PRECISION-factor)*(255*(voter.TrustCount[item.news_type]))/(NUM_NEWS+1) + (128*factor)/PRECISION ;
        }
        return news_pred;
    }

}
