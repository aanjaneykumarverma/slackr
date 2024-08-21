send_text <- function(header,
                      body_text,
                      channel
                      ts = NULL
                      ) {
  slack_api_token <- get_slack_token()
  response <- POST(
    url = "https://slack.com/api/chat.postMessage",
    body = list(
      blocks = list(
        list(
          type = "section",
          text = list(
            type = "mrkdwn",
            text = header
          )
        ),
        list(
          type = "section",
          text = list(
            type = "mrkdwn",
            text = body_text
          )
        )
      ),
      channel = channel,
      thread_ts = ts
    ), 
    add_headers(Authorization = glue("Bearer {slack_api_token}")),
    encode = "json"
  )
  return(content(response)$ts)
}