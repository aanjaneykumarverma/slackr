get_channel_name_from_id <- function(channel_id) {
  slack_api_token <- get_slack_token()
  response <- GET(
    url = "https://slack.com/api/conversations.info",
    query = list(channel = channel_id),
    add_headers(Authorization = glue("Bearer {slack_api_token}"))
  )
  
  # Parse the response
  response_content <- content(response, as = "parsed", type = "application/json")
  
  if (response_content$ok) {
    return(response_content$channel$name)
  } else {
    stop(glue("Failed to retrieve channel name: {response_content$error}"))
  }
}