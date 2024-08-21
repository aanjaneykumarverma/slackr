set_slack_token <- function(token) {
  utils::assignInMyNamespace("SLACK_API_TOKEN", token)
}

get_slack_token <- function() {
  token <- tryCatch(
    get("SLACK_API_TOKEN", envir = asNamespace("yourpackage"), inherits = FALSE),
    error = function(e) NULL
  )
  if (is.null(token)) {
    stop("SLACK_API_TOKEN not set. Please set it before making API calls.")
  }
  return(token)
}