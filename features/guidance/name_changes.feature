@routing @guidance
Feature: Name Changes

    Background:
        Given the profile "guidance"

    Scenario: Different Names
        Given the node map
            | a | b | c | d |

        And the ways
            | nodes | name |
            | ab   | I42 |
            | bc | Street Name (I42) |
            | cd | I42 |

        When I route I should get
            | waypoints | route   | turns |
            | a,d     | ab,bc,cd | head, destination
