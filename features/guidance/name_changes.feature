@routing @guidance
Feature: Name Changes

    Background:
        Given the profile "testbot"

    Scenario: Different Names -- Long in the middle
        Given the node map
            | a | b | c | d |

        And the ways
            | nodes | name |
            | ab   | I42 |
            | bc | Street Name (I42) |
            | cd | I42 |

        When I route I should get
            | waypoints | route   | turns |
            | a,d     | I42 | head,destination |

    Scenario: Different Names -- Long in the middle
        Given the node map
            | a | b | c | d |

        And the ways
            | nodes | name |
            | ab   | Street Name (I42) |
            | bc | I42 |
            | cd | Other Street Name (I42) |

        When I route I should get
            | waypoints | route   | turns |
            | a,d     | I42 | head,destination |
