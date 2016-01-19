@routing @guidance
Feature: Name Changes

    Background:
        Given the profile "testbot"

    Scenario: Different Names -- Long in the middle
        Given the node map
            | a | b | c | d |

        And the ways
            | nodes | name              |
            | ab    | I42               |
            | bc    | Street Name (I42) |
            | cd    | I42               |

        When I route I should get
            | waypoints | route   | turns            |
            | a,d       | I42     | head,destination |

    Scenario: Different Names -- Short in the middle
        Given the node map
            | a | b | c | d |

        And the ways
            | nodes | name                    |
            | ab    | Street Name (I42)       |
            | bc    | I42                     |
            | cd    | Other Street Name (I42) |

        When I route I should get
            | waypoints | route   | turns            |
            | a,d       | I42     | head,destination |

    Scenario: Different Names -- Oszillating
        Given the node map
            | a | b | c | d | e | f | g | h | i |

        And the ways
            | nodes | name                    |
            | ab    | Street Name (I42)       |
            | bc    | I42                     |
            | cd    | Other Street Name (I42) |
            | de    | I42                     |
            | ef    | Other Street Name (I42) |
            | fg    | Street Name (I42)       |
            | gh    | Other Street Name (I42) |
            | hi    | Street Name (I42)       |

        When I route I should get
            | waypoints | route | turns            |
            | a,i       | I42   | head,destination |

    Scenario: Different Names -- Osyillating
        Given the node map
            | a | b | c | d | e | f | g | h | i |

        And the ways
            | nodes | name                    |
            | ab    | Street Name (I42)       |
            | bc    | Other Street Name (I42) |
            | cd    | I42                     |
            | de    | I42                     |
            | ef    | Other Street Name (I42) |
            | fg    | Street Name (I42)       |
            | gh    | Other Street Name (I42) |
            | hi    | Street Name (I42)       |

        When I route I should get
            | waypoints | route | turns            |
            | a,i       | I42   | head,destination |
